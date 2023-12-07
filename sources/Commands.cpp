#include "Server.hpp"

// Join command

void	Server::joinCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	std::vector<std::string>	channelsKeys;
	std::string					channelName;
	std::string					channelKey;
	User*						user = getUser( userSocket );

	if ( user->getIsAuthenticated() == false ) {
		send( user->getSocketFd(), "Server: You must register first\n", 32, 0 );
		return;
	}

	parameters = splitByCharacter( command, ' ' );
	if ( parameters.size() < 2 ) {
		// ERR_NEEDMOREPARAMS 461
		return ;
	}
	if ( parameters.size() == 3 ) {
		std::istringstream	channelKeyStream( parameters.at( 2 ) );

		while ( getline( channelKeyStream, channelKey, ',' ) ) {
			channelsKeys.push_back( channelKey );
		}
	}

	std::istringstream			channelsStream( parameters.at( 1 ) );

	while ( getline( channelsStream, channelName, ',' ) ) {
		if ( isValidChannelName( channelName ) == false ) {
			// ERR_BADCHANMASK 476
			continue ;
		}

		if ( _channels.find( channelName ) == _channels.end() ) 
			createNewChannel( channelName, user );
		else 
			addUserToChannel( channelName, user, channelsKeys );
	}
}

bool	Server::isValidChannelName( std::string& channelName ) {
	return ( channelName[0] == '#' && channelName.length() <= 51 && \
		channelName.find( ' ' ) == std::string::npos && channelName.find( ',' ) == std::string::npos );
}

void	Server::createNewChannel( std::string& channelName, User* user ) {
	Channel channel( channelName );
	
	channel.addUser( user );
	channel.addOperator( user );
	addChannel( channel );

	// send JOIN message to user
	std::string joinMessage( user->getMessagePrefix() + "JOIN " + channelName + "\r\n" );
	
	send ( user->getSocketFd(), joinMessage.c_str(), joinMessage.length(), 0 );
}

void	Server::addUserToChannel( std::string& channelName, User* user, std::vector<std::string>& channelsKeys ) {
	Channel&			channel = _channels[channelName];
	std::string&		key = channelsKeys.at( 0 );
	const std::string&	channelPassword = channel.getPassword();

	if ( channelPassword.empty() == false ) {
		if ( key != channelPassword ) {
			// ERR_BADCHANNELKEY 475
			return ;
		}
	}
	if ( channel.getUserLimit() != -1 && channel.getUserCount() >= channel.getUserLimit() ) {
		// ERR_CHANNELISFULL 471
		return ;
	}
	if ( channel.getInviteOnly() == true ) {
		if ( channel.isInvited( user ) == false ) {
			// ERR_INVITEONLYCHAN 473
			return ;
		}
	}
	
	if ( channelPassword.empty() == false )
		channelsKeys.erase( channelsKeys.begin() );
	channel.removeInvite( user );
	channel.addUser( user );

	// send JOIN message to user
	std::string joinMessage( user->getMessagePrefix() + "JOIN " + channelName + "\r\n" );
	
	send ( user->getSocketFd(), joinMessage.c_str(), joinMessage.length(), 0 );
}

void	Server::passCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );
	
	if ( user->getPasswordStatus() == true ) {
		send ( userSocket, "PASS: You may not reregister\n", 29, 0);
		return;
	}

	std::vector<std::string>	parameters;

	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		send( userSocket, "PASS: Not enough parameters\n", 28, 0 );
		return;
	}

	if ( checkIfPasswordsMatch( parameters.at( 1 ) ) == false ) {
		std::cout << SERVER_INCORRECT_PASSWORD << std::endl;
		send( userSocket, SERVER_INCORRECT_PASSWORD,  56, 0 );
		return;
	}
	user->setPasswordStatusTrue();
	if ( user->getNicknameStatus() == true && user->getUsernameStatus() == true ) {
		std::cout << "here\n";
		user->setIsAuthenticatedTrue();
	}
	send( userSocket, SERVER_CORRECT_PASSWORD, 26, 0 );
}

void	Server::nickCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		send( userSocket, "NICK: No nickname given\n", 24, 0 );
		return;
	}

	User* user = getUser( userSocket );

	if ( findDuplicateNicknames( parameters.at( 1 ) ) == false ) {
		// Need to create a copy of user and create a new one with the new nickname
		// removeUser( user );
		// User*
		user->setNickname( parameters.at( 1 ) );
		if ( user->getNicknameStatus() == false && user->getUsernameStatus() == true
			&& user->getPasswordStatus() == true ) {
				user->setIsAuthenticatedTrue();
		}
		user->setNicknameStatusTrue();

		send( userSocket, SERVER_NICKNAME_ADDED, 24, 0 );
	}
	else
		send( userSocket, SERVER_NICKNAME_ALREADY_IN_USE, 74, 0 );
}

void	Server::userCommand( int userSocket, std::string& command ) {
	User* user = getUser( userSocket );

	if ( user->getUsernameStatus() == true ) {
		send( userSocket, "USER: You may not reregister\n", 29, 0 );
		return;
	}
	
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		send( userSocket, "USER: No username given\n", 24, 0 );
		return;
	}

	user->setUsername( parameters.at( 1 ) );
	user->setUsernameStatusTrue();
	if ( user->getNicknameStatus() == true && user->getPasswordStatus() == true ) {
		user->setIsAuthenticatedTrue();
	}
	send( userSocket, SERVER_USERNAME_ADDED, 24, 0 );
}

void	Server::messageComand( int userSocket, std::string& command ) {
	User*				sender = getUser( userSocket );
	std::string			recipient_name;
	std::string			message;

	if ( sender->getIsAuthenticated() == false ) {
		send( sender->getSocketFd(), "Server: You must register first\n", 32, 0 );
		return;
	}

	message = command.substr( command.find( ':' ) + 1 );
	if ( message.length() == 0 ) {
		//implement error msg
		std::cout << "Message error" << std::endl;
		return ;
	}
	recipient_name = extractNick( command );
	if ( recipient_name.find('#') != std::string::npos ) {
		Channel*	recipient = getChannel( recipient_name );
		if ( recipient == NULL ) {
			// Implement error msg
			std::cout << "Recipient channel error" << std::endl;
			return ;
		}

		std::string	serverMessage( sender->getMessagePrefix() + "PRIVMSG " + recipient_name + " :" + message + "\r\n" );
		recipient->sendMessage( serverMessage, sender->getNickname() );
	}
	else {
		User*	recipient = getUser( recipient_name );
		if ( recipient == NULL ) {
			// Implement error msg
			std::cout << "Recipient user error" << std::endl;
			return ;
		}
		std::string	server_message( sender->getMessagePrefix() + "PRIVMSG " + recipient->getNickname() + " :" + message + "\r\n" );
		send( recipient->getSocketFd(), server_message.c_str(), server_message.size(), 0 );
	}
}

std::string	Server::extractNick( std::string& message ) {
	std::string::size_type	start = message.find( ' ' );
	if ( start != std::string::npos )
	{
		start += 1;
		std::string::size_type	end = message.find( ' ', start );
		if ( end != std::string::npos )
			return ( message.substr( start, end - start ) );
	}
	return ( "" );
}

void	Server::kickCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );

	if ( user->getIsAuthenticated() == false ) { // Checks if user is authenticated
		send( user->getSocketFd(), "Server: You must register first\n", 32, 0 );
		return;
	}

	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 3 ) { // Checks number of parameters given to the command
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), "KICK");
		return;
	}

	if ( _channels.find( parameters.at( 1 ) ) == _channels.end()) { // Checks if channel exists
		ServerMessages::ERR_NOSUCHCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	Channel*	channel = getChannel( parameters.at( 1 ) );

	if ( channel->getUser( user->getNickname() ) == NULL ) { // Checks if user executing command is on the channel
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	if ( channel->getOperator( user->getNickname() ) == NULL ) { // Checks if user executing command is operator in channel
		ServerMessages::ERR_CHANOPRIVSNEEDED( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	User*	target = channel->getUser( parameters.at( 2 ) );

	if ( target == NULL ) { // Checks if target user is on the channel
		ServerMessages::ERR_USERNOTINCHANNEL( userSocket, user->getNickname(), parameters.at( 2 ), parameters.at( 1 ) );
		return;
	}

	channel->ejectUser( target );
}

void	Server::quitCommand( int userSocket, std::string& command ) {

	(void)userSocket;
	
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	std::map<std::string, User>::iterator it = _users.begin();
	std::string	message = "QUIT";

	for (std::vector<std::string>::iterator itP = parameters.begin() + 1; itP != parameters.end(); ++itP)
		message = message + " " + *itP;
	
	removeUser( getUser( userSocket ) );

	for (std::map<std::string, User>::iterator iter = _users.begin(); iter != _users.end(); ++iter) {
		std::cout << "Socket: " << iter->second.getSocketFd() << " name : " << iter->second.getNickname() << std::endl;
	}
	
	for (; it != _users.end(); ++it) { // Sending messages to all users not working
		std::cout <<  it->second.getSocketFd() << std::endl;
		send( it->second.getSocketFd(), message.c_str(), message.size(), 0 );
	}
}
