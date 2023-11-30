#include "Server.hpp"

Server::Server() {}

Server::Server( const Server& copy ) {
	*this = copy;
}

Server::~Server() {}

Server&	Server::operator=( const Server& assign ) {
	if ( this != &assign ) {
		_users = assign._users;
		_serverPassword = assign._serverPassword;
		_serverPort = assign._serverPort;
		_serverSocket = assign._serverSocket;
		_maxSocketFd = assign._maxSocketFd;
		_master = assign._master;
		_readFds = assign._readFds;
		_hints = assign._hints;
	}
	return ( *this );
}

User*	Server::getUser( const std::string& nickname ) {
	std::map< std::string, User >::iterator it = _users.find( nickname );
	if ( it != _users.end() )
		return ( &( it->second ) );
	return ( NULL );
}

User*	Server::getUser( int socketFd ){
	std::map< int, User& >::iterator it = _usersBySocket.find( socketFd );
	if ( it != _usersBySocket.end() )
		return ( &( it->second ) );
	return ( NULL );
}

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

const std::string&	Server::getServerPort() const { return ( _serverPort ); }

Channel*	Server::getChannel( std::string channel ) {
	std::map< std::string, Channel >::iterator it = _channels.find(channel);
	if ( it != _channels.end() )
		return ( &( it->second ) );
	return ( NULL );
}

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

void	Server::setServerPort( const std::string& port ) { _serverPort = port; }

void	Server::addUser( User user ) {
	_users.insert( std::pair<std::string, User>( user.getNickname(), user ) );

	User& userReference = _users.find( user.getNickname() )->second;
	_usersBySocket.insert( std::pair<int, User&>( userReference.getSocketFd(),  userReference ) );
}

void	Server::addChannel( Channel& channel ) {
	_channels.insert( std::pair<std::string, Channel>( channel.getName(), channel ) );
}

void	Server::removeUser( User& user ) {
	_usersBySocket.erase( user.getSocketFd() );
	_users.erase( user.getNickname() );
}

long	Server::simpleHash( std::string& command ) {
	std::string::iterator	iter;
	long					hash = 0;

	iter = command.begin();
	for ( int index = 0; iter != command.end() && *iter != ' '; ++iter, ++index ) {
		hash += ( index + 1 ) * *iter;
	}
	return ( hash );
}

bool	Server::checkIfPasswordsMatch( const std::string& input ) const {
	return ( _serverPassword.compare( input ) == 0 );
}

bool	Server::findDuplicateNicknames( const std::string& nickname ) const {
	if ( _users.find( nickname ) == _users.end() )
		return ( false );
	return ( true );
}

void	Server::selectCommand( int userSocket, std::string& command ) {
	switch ( simpleHash( command ) ) {
		case QUIT:
			break;
		
		case JOIN:
			joinCommand( userSocket, command);
			break;
		
		case KICK:
			break;
		
		case INVITE:
			break;

		case TOPIC:
			break;
		
		case MODE:
			break;

		case PASS:
			passCommand( userSocket, command );
			break;

		case NICK:
			nickCommand( userSocket, command );
			break;

		case USER:
			userCommand( userSocket, command );
			break;

		case PRIVMSG:
			messageComand( userSocket, command );
			break;
		
		default:
			std::cout << "HASH: " << simpleHash( command ) << std::endl;
			std::cout << "COMMAND: " << command << std::endl;
			break;
	}
}

std::vector<std::string>	Server::getParameters( std::string& command ) {
	std::vector<std::string>	parameters;
	std::istringstream			commandStream( command );
	std::string					tempString;

	while ( getline( commandStream, tempString, ' ') )
		parameters.push_back( tempString );
	
	return ( parameters );
}

void	Server::joinCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	std::vector<std::string>	channelsKeys;
	std::string					channelName;
	std::string					channelKey;
	User*						user = getUser( userSocket );

	parameters = getParameters( command );
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
	std::istringstream			f( command );
	std::string					string;

	while ( getline( f, string, ' ' ) )
		parameters.push_back( string );

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
	send( userSocket, SERVER_CORRECT_PASSWORD, 26, 0 );
}

void	Server::nickCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	std::istringstream			f( command );
	std::string					string;

	while ( getline( f, string, ' ' ) )
		parameters.push_back( string );

	if ( parameters.size() < 2 ) {
		send( userSocket, "NICK: No nickname given\n", 24, 0 );
		return;
	}

	User& user = *getUser( userSocket );

	user.setNickname("ola mano");

	if ( findDuplicateNicknames( parameters.at( 1 ) ) == false ) {

		User	newUser( user );
		newUser.setNickname( parameters.at( 1 ) );

		removeUser( user );
		addUser( newUser );

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
	std::istringstream			f( command );
	std::string					string;

	while ( getline( f, string, ' ' ) )
		parameters.push_back( string );

	if ( parameters.size() < 2 ) {
		send( userSocket, "USER: No username given\n", 24, 0 );
		return;
	}

	user->setUsername( parameters.at( 1 ) );
	send( userSocket, SERVER_USERNAME_ADDED, 24, 0 );
}

void	Server::messageComand( int userSocket, std::string& command ) {
	User*				sender;
	std::string			recipient_name;
	std::string			message;

	message = command.substr( command.find( ':' ) + 1 );
	if ( message.length() == 0 ) {
		//implement error msg
		std::cout << "Message error" << std::endl;
		return ;
	}
	sender = getUser( userSocket );
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

