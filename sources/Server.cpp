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

User&	Server::getUser( const std::string& getnickname ) { return ( _users[getnickname] ); }

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

bool	Server::checkIfPasswordsMatch(const std::string& input ) const {
	int	passCommandLength = strlen( "PASS " );

	return ( _serverPassword.compare( input.substr( passCommandLength, input.length() - passCommandLength - 1 ) ) == 0 );
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
			passCommand( userSocket, command);
			break;

		case NICK:
			nickCommand( userSocket, command);
			break;

		case USER:
			userCommand( userSocket, command);
			break;
		
		default:
			break;
	}
}

void	Server::joinCommand( int userSocket, std::string& command ) {
	std::string									channelName;
	std::map<std::string, Channel>::iterator	iter;
	User*										user;

	channelName = command.substr( 5, command.length() - 5 );
	iter = _channels.find( channelName );
	user = getUser( userSocket );

	if ( iter == _channels.end() )
		createNewChannel( channelName, user );
		
}

void	Server::createNewChannel( std::string& channelName, User* user ) {
	Channel channel( channelName );
	

	channel.addUser( user );
	channel.addOperator( user );
	addChannel( channel );
}

void	Server::passCommand( int userSocket, std::string& command ) {
	User*	user = getUser ( userSocket );
	
	if ( user->getPasswordStatus() == true ) {
		send ( userSocket, "PASS: You may not reregister\n", 23, 0);
		return;
	}

	std::vector<std::string>	parameters;
    std::istringstream			f( command );
    std::string					string;

    while ( getline( f, string, ' ' ) )
		parameters.push_back( string );

	if ( parameters.size() < 2 ) {
		send( userSocket, "PASS: Not enough parameters\n", 35, 0 );
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

	User* user = getUser( userSocket );

	if ( findDuplicateNicknames( parameters.at( 1 ) ) == false ) {
		user->setNickname( parameters.at( 1 ) ); ;
		send( userSocket, SERVER_NICKNAME_ADDED, 24, 0 );
	}
	else
		send( userSocket, SERVER_NICKNAME_ALREADY_IN_USE, 74, 0 );
}

void	Server::userCommand( int userSocket, std::string& command ) {
	if ( getUser( userSocket )->getUsernameStatus() == true ) {
		send (userSocket, "USER: You may not reregister\n", 23, 0);
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

	User* user = getUser( userSocket );

	user->setUsername( parameters.at( 1 ) );	
			send( userSocket, SERVER_USERNAME_ADDED, 24, 0 );
}