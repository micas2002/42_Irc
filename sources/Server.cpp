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

User&	Server::getUser( const std::string& username ) { return ( _users[username] ); }

User*	Server::getUser( int socketFd ) { return ( _usersBySocket[socketFd] ); }

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

const std::string&	Server::getServerPort() const { return ( _serverPort ); }

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

void	Server::setServerPort( const std::string& port ) { _serverPort = port; }

void	Server::addUser( User& user ) {
	_users.insert( std::pair<std::string, User>( user.getNickname(), user ) );
	_usersBySocket.insert( std::pair<int, User*>( user.getSocketFd(),  &user ) );
}

void	Server::addChannel( Channel& channel ) {
	_channels.insert( std::pair<std::string, Channel>( channel.getName(), channel ) );
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
			break;

		case NICK:
			break;

		case USER:
			break;
		
		default:
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
			createNewChannel( channelName, getUser( userSocket ) );
		else 
			addUserToChannel( channelName, getUser( userSocket ), channelsKeys );
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

}

void	Server::addUserToChannel( std::string& channelName, User* user, std::vector<std::string>& channelsKeys ) {
	Channel&		channel = _channels[channelName];
	std::string&	key = channelsKeys.at( 0 );
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
	if ( channel.getInviteOnly() == true) {
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
}