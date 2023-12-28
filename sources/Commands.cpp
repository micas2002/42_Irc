#include "Server.hpp"

// Join command
void	Server::joinCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	std::vector<std::string>	channelsKeys;
	std::string					channelName;
	std::string					channelKey;
	User*						user = getUser( userSocket );

	if ( user->getIsAuthenticated() == false ) {
		ServerMessages::ERR_NOTREGISTERED( userSocket, user->getNickname() );
		return;
	}

	parameters = splitByCharacter( command, ' ' );
	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), parameters.at( 0 ) ); // ERR_NEEDMOREPARAMS 461
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
			ServerMessages::ERR_BADCHANMASK( userSocket, user->getNickname(), parameters.at( 0 ) ); // ERR_BADCHANMASK 476
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
	user->addChannel( channelName, &_channels[channelName]);

	ServerMessages::JOIN_MESSAGE( user->getSocketFd(), user, channelName );
}

void	Server::addUserToChannel( std::string& channelName, User* user, std::vector<std::string>& channelsKeys ) {
	Channel&			channel = _channels[channelName];
	std::string			key;
	const std::string&	channelPassword = channel.getPassword();

	if ( channelPassword.empty() == false ) {
		if ( channelsKeys.size() == 0 || ( key = channelsKeys.at( 0 ) ) != channelPassword ) {
			ServerMessages::ERR_BADCHANNELKEY( user->getSocketFd(), user->getNickname(), channelName ); // ERR_BADCHANNELKEY 475
			return ;
		}
	}
	if ( channel.getUserLimit() != -1 && channel.getUserCount() >= channel.getUserLimit() ) {
		ServerMessages::ERR_CHANNELISFULL( user->getSocketFd(), user->getNickname(), channelName ); // ERR_CHANNELISFULL 471
		return ;
	}
	if ( channel.getInviteOnly() == true ) {
		if ( channel.isInvited( user->getNickname() ) == false ) {
			ServerMessages::ERR_INVITEONLYCHAN( user->getSocketFd(), user->getNickname(), channelName ); // ERR_INVITEONLYCHAN 473
			return ;
		}
	}
	
	if ( channelPassword.empty() == false )
		channelsKeys.erase( channelsKeys.begin() );
	channel.removeInvite( user );
	channel.addUser( user );
	user->addChannel( channelName, &channel );

	std::map<std::string, User*>::iterator	iter = channel.getUsers().begin();
	for ( ; iter != channel.getUsers().end(); ++iter ) {
		ServerMessages::JOIN_MESSAGE( iter->second->getSocketFd(), user, channelName );
	}
}

// PASS command
void	Server::passCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );
	
	if ( user->getPasswordStatus() == true ) {
		ServerMessages::ERR_ALREADYREGISTERED( userSocket, user->getNickname() );
		return;
	}

	std::vector<std::string>	parameters;

	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), command );
		return;
	}

	if ( checkIfPasswordsMatch( parameters.at( 1 ) ) == false ) {
		ServerMessages::ERR_PASSWDMISMATCH( userSocket, user->getNickname() );
		return;
	}
	user->setPasswordStatusTrue();
	if ( user->getNicknameStatus() == true && user->getUsernameStatus() == true ) {
		user->setIsAuthenticatedTrue();
	}
	send( userSocket, SERVER_CORRECT_PASSWORD, 26, 0 );
}

// NICK command 
void	Server::nickCommand( int userSocket, std::string& command ) {
	User*						user = getUser( userSocket );
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NONICKNAMEGIVEN( userSocket, user->getNickname() );
		return;
	}

	if ( findDuplicateNicknames( parameters.at( 1 ) ) == false ) {
		User	updatedUser( *user );

		removeUser( user );
		updatedUser.setNickname( parameters.at( 1 ) );
		addUser( updatedUser );

		user = getUser( userSocket );
		if ( user->getNicknameStatus() == false && user->getUsernameStatus() == true
			&& user->getPasswordStatus() == true ) {
				user->setIsAuthenticatedTrue();
		}
		user->setNicknameStatusTrue();

		send( userSocket, SERVER_NICKNAME_ADDED, 24, 0 );
	}
	else
		ServerMessages::ERR_NICKNAMEINUSE( userSocket, user->getNickname(), parameters.at( 1 ) );
}

// USER command
void	Server::userCommand( int userSocket, std::string& command ) {
	User* user = getUser( userSocket );

	if ( user->getUsernameStatus() == true ) {
		ServerMessages::ERR_ALREADYREGISTERED( userSocket, user->getNickname() );
		return;
	}
	
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), command );
		return;
	}

	user->setUsername( parameters.at( 1 ) );
	user->setUsernameStatusTrue();
	if ( user->getNicknameStatus() == true && user->getPasswordStatus() == true ) {
		user->setIsAuthenticatedTrue();
	}
	send( userSocket, SERVER_USERNAME_ADDED, 24, 0 );
}

// MESSAGE command
void	Server::messageComand( int userSocket, std::string& command ) {
	User*				sender = getUser( userSocket );
	std::string			recipient_name;
	std::string			message;

	// TO DO:
	// need to implement errors 404, and 411

	if ( sender->getIsAuthenticated() == false ) {
		ServerMessages::ERR_NOTREGISTERED( userSocket, sender->getNickname() );
		return;
	}

	message = command.substr( command.find( ':' ) + 1 );
	if ( message.length() == 0 ) {
		ServerMessages::ERR_NOTEXTTOSEND( userSocket, sender->getNickname() );
		return ;
	}
	recipient_name = extractNick( command );
	if ( recipient_name.find('#') != std::string::npos ) {
		Channel*	recipient = getChannel( recipient_name );
		if ( recipient == NULL ) {
			ServerMessages::ERR_NOSUCHNICK( userSocket, sender->getNickname(), recipient_name );
			return ;
		}

		std::string	serverMessage( sender->getMessagePrefix() + "PRIVMSG " + recipient_name + " :" + message + "\r\n" );
		recipient->sendMessage( serverMessage, sender->getNickname() );
	}
	else {
		User*	recipient = getUser( recipient_name );
		if ( recipient == NULL ) {
			ServerMessages::ERR_NOSUCHNICK( userSocket, sender->getNickname(), recipient_name );
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

// KICK command
void	Server::kickCommand( int userSocket, const std::string& command ) {
	User*	user = getUser( userSocket );

	if ( user->getIsAuthenticated() == false ) { // Checks if user is authenticated
		ServerMessages::ERR_NOTREGISTERED( userSocket, user->getNickname() );
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

// QUIT command
void	Server::quitCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	std::string	message = "QUIT";

	for (std::vector<std::string>::iterator itP = parameters.begin() + 1; itP != parameters.end(); ++itP)
		message = message + " " + *itP;
	
	User*										user = getUser( userSocket );
	std::map<std::string, Channel*>				channelsMap = user->getChannels();
	std::map<std::string, Channel*>::iterator	chanIt = channelsMap.begin();
	Channel*									channel;

	for (; chanIt != channelsMap.end(); ++chanIt ) { // Removes user from channels he is part of
		channel = getChannel( chanIt->first );
		channel->ejectUser( user );
	}

	removeUser( getUser( userSocket ) ); // Removes user from server

	std::map<std::string, User>::iterator	it = _users.begin();
	
	for (; it != _users.end(); ++it) { // Sending messages to all users not working
		std::cout <<  it->second.getSocketFd() << std::endl;
		send( it->second.getSocketFd(), message.c_str(), message.size(), 0 );
	}
}

// WHO command
void	Server::whoCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );
	
	if ( user->getIsAuthenticated() == false ) {
		ServerMessages::ERR_NOTREGISTERED( userSocket, user->getNickname() );
		return ;
	}

	std::vector<std::string>	parameters;
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), parameters.at( 0 ) ); // ERR_NEEDMOREPARAMS 461
		return ;
	}

	const std::string& target = parameters.at( 1 );
	if ( target.at( 0 ) == '#' ) {
		whoChannel( userSocket, target );
	}
	else {
		whoUser( userSocket, target );
	}
}

void	Server::whoChannel( int userSocket, const std::string& channelName ) {
	User*		user = getUser( userSocket );
	Channel*	channel = getChannel( channelName );

	if ( channel == NULL ) {
		ServerMessages::RPL_ENDOFWHO( userSocket, user->getNickname(), channelName ); // RPL_ENDOFWHO 315
		return ;
	}

	const std::map<std::string, User*>&	users = channel->getUsers();
	std::map<std::string, User*>::const_iterator	iter = users.begin();
	for ( ; iter != users.end(); ++iter ) {
		ServerMessages::RPL_WHOREPLY( userSocket, iter->second, user->getNickname(), channelName ); // RPL_WHOREPLY 352
	}
	ServerMessages::RPL_ENDOFWHO( userSocket, user->getNickname(), channelName ); // RPL_ENDOFWHO 315
}

void	Server::whoUser( int userSocket, const std::string& username ) {
	User* user = getUser( userSocket );
	User* target = getUser( username );

	if ( target != NULL) {
		ServerMessages::RPL_WHOREPLY( userSocket, target, user->getNickname(), username ); // RPL_WHOREPLY 352
	}
	ServerMessages::RPL_ENDOFWHO( userSocket, user->getNickname(), username ); // RPL_ENDOFWHO 315
}

// INVITE command
void	Server::inviteCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );
	if ( parameters.size() < 3 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), parameters.at( 0 ) ); // ERR_NEEDMOREPARAMS 461
		return ;
	}

	Channel* channel = getChannel( parameters.at( 2 ) );
	if ( channel == NULL ) {
		ServerMessages::ERR_NOSUCHCHANNEL( userSocket, user->getNickname(), parameters.at( 2 ) ); // ERR_NOSUCHCHANNEL 403
		return ;
	}

	if ( channel->isUser( user->getNickname() ) == false ) {
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 2 ) ); // ERR_NOTONCHANNEL 442
		return ;
	}

	if ( channel->isOperator( user->getNickname() ) == false ) {
		ServerMessages::ERR_CHANOPRIVSNEEDED( userSocket, user->getNickname(), parameters.at( 2 ) ); // ERR_CHANOPRIVSNEEDED 482
		return ;
	}

	if ( channel->isUser( parameters.at( 1 ) ) == true ) {
		ServerMessages::ERR_USERNOTINCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ), parameters.at( 2 ) ); // ERR_USERNOTINCHANNEL 441
		return ;
	}

	User*	target = getUser( parameters.at( 1 ) );
	if ( target == NULL ) {
		ServerMessages::ERR_NOSUCHNICK( userSocket, user->getNickname(), parameters.at( 1 ) ); // ERR_NOSUCHNICK 401
		return ;
	}

	channel->addInvite( target );
	ServerMessages::RPL_INVITING( userSocket, user->getNickname(), target->getNickname(), channel->getName() ); // RPL_INVITING 341
	ServerMessages::INVITE_MESSAGE( target->getSocketFd(), user, target->getNickname(), channel->getName() ); // INVITE_MESSAGE
}