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

User*	Server::getUser( int socketFd ) {
	return ( getUser( _usersBySocket[ socketFd ] ) );
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

	_usersBySocket.insert( std::pair<int, std::string>( user.getSocketFd(),  user.getNickname() ) );
}

void	Server::addChannel( Channel& channel ) {
	_channels.insert( std::pair<std::string, Channel>( channel.getName(), channel ) );
}

void	Server::removeUser( User& user ) {
	_usersBySocket.erase( user.getSocketFd() );
	_users.erase( user.getNickname() );
}

std::vector<std::string>	Server::splitByCharacter( const std::string& input, char c ) {
	std::vector<std::string>	parameters;
	std::istringstream			f( input );
	std::string					string;

	while ( getline( f, string, c ) )
		parameters.push_back( string );

	return (parameters);
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
