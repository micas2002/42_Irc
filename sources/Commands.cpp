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

		if ( _channels.find( channelName ) == _channels.end() ) {
			createNewChannel( channelName, user );
		}
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
	channel.addOperator( user->getNickname() );
	addChannel( channel );
	user->addChannel( channelName, &_channels[channelName]);

	ServerMessages::JOIN_MESSAGE( user->getSocketFd(), user, &channel );
	ServerMessages::RPL_NAMREPLY( user->getSocketFd(), user, &channel );
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
	channel.removeInvite( user->getNickname() );
	channel.addUser( user );
	user->addChannel( channelName, &channel );

	std::map<std::string, User*>::iterator	iter = channel.getUsers().begin();
	for ( ; iter != channel.getUsers().end(); ++iter ) {
		ServerMessages::JOIN_MESSAGE( iter->second->getSocketFd(), user, &channel );
	}
	ServerMessages::RPL_NAMREPLY( user->getSocketFd(), user, &channel );
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
	std::string					old_nick = user->getNickname();
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NONICKNAMEGIVEN( userSocket, user->getNickname() );
		return;
	}

	if ( findDuplicateNicknames( parameters.at( 1 ) ) == false ) {
		User					updatedUser( *user );
		std::vector< Channel* > regularChannels = user->getRegularChannels();
		std::vector< Channel* > operatorChannels = user->getOperatorChannels();

		removeUser( user );
		updatedUser.setNickname( parameters.at( 1 ) );
		addUser( updatedUser );

		user = getUser( userSocket );
		changeNickInChannel( user, regularChannels, operatorChannels );
		
		if ( user->getNicknameStatus() == false && user->getUsernameStatus() == true
			&& user->getPasswordStatus() == true ) {
				user->setIsAuthenticatedTrue();
		}
		user->setNicknameStatusTrue();

		send( userSocket, SERVER_NICKNAME_ADDED, 24, 0 );
		ServerMessages::NICK_MESSAGE( userSocket, user, parameters.at( 1 ), old_nick );
	}
	else
		ServerMessages::ERR_NICKNAMEINUSE( userSocket, user->getNickname(), parameters.at( 1 ) );
}

void	Server::changeNickInChannel( User* newUser, std::vector< Channel* >& regularChannels, std::vector< Channel* >& operatorChannels ) {
	std::vector< Channel* >::iterator		chanIt = regularChannels.begin();
	Channel* channel;
	for (; chanIt != regularChannels.end(); ++chanIt ) {
		channel = getChannel( (*chanIt)->getName() );
		channel->addUser( newUser );
		newUser->addChannel( channel->getName(), channel );
	}
	chanIt = operatorChannels.begin();
	for (; chanIt != operatorChannels.end(); ++chanIt ) {
		channel = getChannel( (*chanIt)->getName() );
		channel->addOperator( newUser->getNickname() );
		channel->addUser( newUser );
		newUser->addChannel( channel->getName(), channel );
	}
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

	if ( channel->isUser( user->getNickname() ) == false ) { // Checks if user executing command is on the channel
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	if ( channel->isOperator( user->getNickname() ) == false ) { // Checks if user executing command is operator in channel
		ServerMessages::ERR_CHANOPRIVSNEEDED( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	User*	target = channel->getUser( parameters.at( 2 ) );

	if ( target == NULL ) { // Checks if target user is on the channel
		ServerMessages::ERR_USERNOTINCHANNEL( userSocket, user->getNickname(), parameters.at( 2 ), parameters.at( 1 ) );
		return;
	}

	channel->ejectUser( target->getNickname() );
	std::string comment = ":Because you stink ";
	
	if ( parameters.size() > 3 && parameters.at( 3 ).at( 0 ) == ':' ) {
		comment.clear();
		for ( std::vector< std::string >::iterator it = parameters.begin() + 3; it != parameters.end(); it++)
			comment += *it + " ";
	}
	comment.erase(comment.end() - 1);
	ServerMessages::KICK_MESSAGE( userSocket, user, target->getNickname(),  channel, comment );
}

// PART command
void	Server::partCommand( int userSocket, std::string& command ) {
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

	if ( channel->isUser( user->getNickname() ) == false ) { // Checks if user executing command is on the channel
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	channel->ejectUser( user->getNickname() );
	ServerMessages::PART_MESSAGE( userSocket, user,  channel );
}

// QUIT command
void	Server::quitCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	std::string	message = "QUIT";

	for (std::vector<std::string>::iterator itP = parameters.begin() + 1; itP != parameters.end(); ++itP)
		message = message + " " + *itP;
	
	User*										user = getUser( userSocket );
	std::map<std::string, Channel*>				channelsMap = user->getAllChannels();
	std::map<std::string, Channel*>::iterator	chanIt = channelsMap.begin();
	Channel*									channel;

	for (; chanIt != channelsMap.end(); ++chanIt ) { // Removes user from channels he is part of
		channel = getChannel( chanIt->first );
		channel->ejectUser( user->getNickname() );
	}
	_commandInput.erase( userSocket );

	removeUser( getUser( userSocket ) ); // Removes user from server

	std::map<std::string, User>::iterator	it = _users.begin();
	
	for (; it != _users.end(); ++it) { // Sending messages to all users not working
		std::cout <<  it->second.getSocketFd() << std::endl;
		send( it->second.getSocketFd(), message.c_str(), message.size(), 0 );
	}
	close( userSocket );	// close the fd!
	FD_CLR( userSocket, &_master ) ; // remove fd from master set
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

// MODE Command
void	Server::modeCommand( int userSocket, std::string& command ) {
	User*						sender = getUser( userSocket );
	std::vector< std::string >	params = splitByCharacter( command, ' ' );

	if ( params.at( 1 ).find( '#' ) != std::string::npos ) {
		Channel*	ch = getChannel( params.at( 1 ) );
		if (!ch) {
			ServerMessages::ERR_NOSUCHCHANNEL( userSocket, sender->getNickname(), params.at( 1 ) );
			return ;
		}
		modeChannel( sender, params, ch );
	}

}

void	Server::modeChannel( User* sender, std::vector< std::string > params, Channel* ch ) {

	if ( params.size() == 2 ) { // Return all the active modes for the channel
		ServerMessages::RPL_CHANNELMODEIS( sender->getSocketFd(), sender, ch->getName(), ch->getModes() );
		return ;
	}

	std::string					rawModes = params.at( 2 );
	std::vector< std::string >	modeChanges;
	std::string					modifier = "+";

	for ( size_t i = 0; i < rawModes.length(); i++ ) {
		if ( rawModes[i] == '+' || rawModes[i] == '-' ) {
			modifier[0] = rawModes[i];
			continue ;
		}
		modeChanges.push_back( (modifier + rawModes[i]) );
	}

	if ( modeChanges.size() == 0 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( sender->getSocketFd(), sender->getNickname(), "MODE" );
		return ;
	}

	if ( ch->isOperator( sender->getNickname() ) == false ) {
		ServerMessages::ERR_CHANOPRIVSNEEDED( sender->getSocketFd(), sender->getNickname(), ch->getName() );
		return ;
	}

	int	paramsCounter = 3;
	std::string argument;

	for ( std::vector< std::string >::iterator it = modeChanges.begin(); it != modeChanges.end(); it++ ) {
		
		std::string tmp = *it;
		switch ( tmp.at(1) )
		{
			case MODE_I:
				modeInvite( ch, *it, sender );
				break;
			
			case MODE_T:
				modeTopic( ch, *it, sender );
				break;

			case MODE_K:
				if ( it->find( '-' ) != std::string::npos ) {
					modeKey( ch, *it, sender, "" );
					break;
				}
				try {
					argument = params.at( paramsCounter++ );
					modeKey( ch, *it, sender, argument );
				} catch ( std::out_of_range &e ) {
					ServerMessages::ERR_NEEDMOREPARAMS( sender->getSocketFd(), sender->getNickname(), "MODE" );
					return ;
				}
				break;

			case MODE_O:
				try {
					argument = params.at( paramsCounter++ );
					modeOperator( ch, *it, sender, argument );
				} catch ( std::out_of_range &e ) {
					ServerMessages::ERR_NEEDMOREPARAMS( sender->getSocketFd(), sender->getNickname(), "MODE" );
					return ;
				}
				break;

			case MODE_L:
				if ( it->find( '-' ) != std::string::npos ) {
					modeLimit( ch, *it, sender, "" );
					break;
				}
				try {
					argument = params.at( paramsCounter++ );
					modeLimit( ch, *it, sender, argument );
				} catch ( std::out_of_range &e ) {
					ServerMessages::ERR_NEEDMOREPARAMS( sender->getSocketFd(), sender->getNickname(), "MODE" );
					return ;
				}
				break;

			default:
				ServerMessages::ERR_NEEDMOREPARAMS( sender->getSocketFd(), sender->getNickname(), "MODE" );
				break;
		}
	}
}

void	Server::modeInvite( Channel* ch, std::string flag, User* user ) {
	flag.find( '+' ) != std::string::npos ? ch->setInviteOnly( true ) : ch->setInviteOnly( false );
	modeMessage( user, ch->getName(), flag, "" );
}

void	Server::modeTopic( Channel* ch, std::string flag, User* user ) {
	flag.find( '+' ) != std::string::npos ? ch->setTopicRestriction( true ) : ch->setTopicRestriction( false ); 
	modeMessage( user, ch->getName(), flag, "" );	
}

void	Server::modeKey( Channel* ch, std::string flag, User* user, std::string newKey ) {
	ch->setPassword( newKey );
	modeMessage( user, ch->getName(), flag, newKey );
}

void	Server::modeOperator( Channel *channel, std::string flag, User* sender, std::string receiver ) {
	User*	rec = getUser( receiver );
	if ( !rec ) {
		ServerMessages::ERR_USERNOTINCHANNEL( sender->getSocketFd(), sender->getNickname(), rec->getNickname(), channel->getName() );
		return ;
	}
	if ( flag.find( '+' ) != std::string::npos )
		channel->addOperator( rec->getNickname() );
	else
		channel->ejectOperator( rec->getNickname() );
	modeMessage( sender, channel->getName(), flag, receiver );
}

void	Server::modeLimit( Channel *channel, std::string flag, User* sender, std::string limit ) {
	int	newLimit;
	if ( limit.length() == 0 )
		newLimit = -1;
	else {	
		newLimit = atoi( limit.c_str() );
		if ( newLimit <= 0 )
			return ;
	}
	channel->setUserLimit( newLimit );
	modeMessage( sender, channel->getName(), flag, limit );
}

void	Server::modeMessage( User* user, const std::string& channel_name, const std::string& modes, const std::string& arguments ) {
	std::string	serverMessage( user->getMessagePrefix() + "MODE " + channel_name + " " + modes + " " + arguments + "\r\n" );
	send( user->getSocketFd(), serverMessage.c_str(), serverMessage.size(), 0 );
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

	channel->addInvite( target->getNickname() );
	ServerMessages::RPL_INVITING( userSocket, user->getNickname(), target->getNickname(), channel->getName() ); // RPL_INVITING 341
	ServerMessages::INVITE_MESSAGE( target->getSocketFd(), user, target->getNickname(), channel->getName() ); // INVITE_MESSAGE
}

// TOPIC command
void	Server::topicCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );
	std::vector<std::string>	parameters;

	parameters = splitByCharacter( command, ' ' );
	if ( parameters.size() < 2 ) {
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), parameters.at( 0 ) ); // ERR_NEEDMOREPARAMS 461
		return ;
	}

	Channel*	channel = getChannel( parameters.at( 1 ) );
	if ( channel == NULL ) {
		ServerMessages::ERR_NOSUCHCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) ); // ERR_NOSUCHCHANNEL 403
		return ;
	}

	if ( channel->isUser( user->getNickname() ) == false ) {
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) ); // ERR_NOTONCHANNEL 442
		return ;
	}

	if ( parameters.size() == 2 ) {
		const std::string&	topic = channel->getTopic();
		if ( topic.empty() ) {
			ServerMessages::RPL_NOTOPIC( userSocket, user->getNickname(), channel->getName() ); // RPL_NOTOPIC 331
		}
		else {
			ServerMessages::RPL_TOPIC( userSocket, user->getNickname(), channel->getName(), topic ); // RPL_TOPIC 332
		}
		return ;
	}

	if ( channel->getTopicRestriction() && !channel->isOperator( user->getNickname() ) ) {
		ServerMessages::ERR_CHANOPRIVSNEEDED( userSocket, user->getNickname(), parameters.at( 1 ) ); // ERR_CHANOPRIVSNEEDED 482
		return ;
	}

	std::string& topic = parameters.at( 2 );
	topic.erase( 0, 1 );
	channel->setTopic( topic );
	ServerMessages::RPL_TOPIC( userSocket, user->getNickname(), channel->getName(), topic ); // RPL_TOPIC 332
}

// NAMES command
void	Server::namesCommand( int userSocket, std::string& command ) {
	User*	user = getUser( userSocket );

	if ( user->getIsAuthenticated() == false ) { // Checks if user is authenticated
		ServerMessages::ERR_NOTREGISTERED( userSocket, user->getNickname() );
		return;
	}

	std::vector<std::string>	parameters;
	
	parameters = splitByCharacter( command, ' ' );

	if ( parameters.size() < 2 ) { // Checks number of parameters given to the command
		ServerMessages::ERR_NEEDMOREPARAMS( userSocket, user->getNickname(), "KICK");
		return;
	}

	if ( _channels.find( parameters.at( 1 ) ) == _channels.end()) { // Checks if channel exists
		ServerMessages::ERR_NOSUCHCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	Channel*	channel = getChannel( parameters.at( 1 ) );

	if ( channel->isUser( user->getNickname() ) == false ) { // Checks if user executing command is on the channel
		ServerMessages::ERR_NOTONCHANNEL( userSocket, user->getNickname(), parameters.at( 1 ) );
		return;
	}

	ServerMessages::RPL_NAMREPLY( userSocket, user, channel );
}