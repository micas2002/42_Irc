#include "Server.hpp"


void	Server::createAndBindSocket() {
	FD_ZERO( &_master );    // clear the master and temp sets
	FD_ZERO( &_readFds );

	memset( &_hints, 0, sizeof( _hints ) );
	_hints.ai_family = AF_INET6;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE; // use my IP

	int					addrInfoReturn;
	struct addrinfo*	serverInfo;
	if ( ( addrInfoReturn = getaddrinfo( NULL, _serverPort.c_str(), &_hints, &serverInfo ) ) != 0 )
		throw std::runtime_error( gai_strerror( addrInfoReturn ) );

	// loop through all the results and bind to the first we can
	int 				yes = 1; // used to tell setsockopt(  ) that the flag SO_REUSEADDR should be enabled
	struct addrinfo*	addrPointer;
	for( addrPointer = serverInfo; addrPointer != NULL; addrPointer = addrPointer->ai_next ) {
		if ( ( _serverSocket = socket( addrPointer->ai_family, addrPointer->ai_socktype, addrPointer->ai_protocol ) ) == -1 ) {
			std::perror( "Server: socket" );
			continue;
		}

		if ( setsockopt( _serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
			std::perror( "setsockopt" );
			exit( 1 );
		}

		if ( bind( _serverSocket, addrPointer->ai_addr, addrPointer->ai_addrlen ) == -1 ) {
			close( _serverSocket );
			std::perror( "Server: bind" );
			continue;
		}
		break;
	}
	freeaddrinfo( serverInfo );

	if ( addrPointer == NULL )
		throw std::runtime_error( "failed to bind" );

	if ( listen( _serverSocket, 0 ) == -1 )
		throw std::runtime_error( "failed to listen" );
			
	FD_SET( _serverSocket, &_master );
	// keep track of the biggest file descriptor
	_maxSocketFd = _serverSocket; // so far, it's this one
	std::cout << SERVER_AWAITING_CONNECTIONS << std::endl;
	serverLoop();
}

void	Server::serverLoop() {
	while( 1 ) {
		_readFds = _master; // copy read_fds into master
		if ( select( _maxSocketFd + 1, &_readFds, NULL, NULL, NULL ) == -1 ) {
			perror( "select" );
			exit(4);
		}

		for( int index = 3; index <= _maxSocketFd; index++ ) {
			if ( FD_ISSET( index, &_readFds ) )// we found one fd that is ready to be read
			{
				if ( index == _serverSocket ) // new user trying to connect
					handleNewConnection();
				else 
					handleClientData( index );
			}
		}
	}
}

void	Server::handleNewConnection() {
	// handle new connections
	socklen_t			sinSize;
	int					newFd;
	struct sockaddr_in6	theirAddr;
	char 				ip6[INET6_ADDRSTRLEN];

	sinSize = sizeof( struct sockaddr_storage );
	newFd = accept( _serverSocket, ( struct sockaddr * )&theirAddr, &sinSize );

	if ( newFd == -1 )
		perror( "accept" );
	else
	{
		std::cout << "Server: socket " << newFd << " is attempting to connect." << std::endl;
		send( newFd, "Welcome to our IRC server. Please input username, nickname and server password.\n", 80, 0 );
		send( newFd, "Use /NICK, /USER and /PASS commands.\n", 37, 0 );

		FD_SET( newFd, &_master ); // add to master set

		if ( newFd > _maxSocketFd )
				_maxSocketFd = newFd;
			
		inet_ntop( AF_INET6, &( theirAddr.sin6_addr ), ip6, INET6_ADDRSTRLEN );
		std::cout << "Server: " << ip6 << " successfully connected to socket " << newFd << "." << std::endl;

		std::ostringstream oss;
		oss << "Default " << newFd;
		User user( oss.str() );
		user.setSocketFd( newFd );
		addUser( user );
	}
}

void	Server::handleClientData( int clientSocket ) {
	char	buffer[256]; // buffer for client data
	int		numberOfBytes;

	// handle data from a client
	if ( (numberOfBytes = recv( clientSocket, buffer, sizeof( buffer ), 0 ) ) <= 0 ) {// got error or connection closed by client
		if ( numberOfBytes == 0 ) // connection closed
			std::cout << "selectserver: socket " << clientSocket << " hung up" << std::endl;
		else
			perror( "recv" );
		close( clientSocket ); // close the fd!
		FD_CLR( clientSocket, &_master ); // remove fd from master set
		return ;
	}
	// we got some data from a client
	std::string	command( buffer );

	// command.erase( --command.end() );
	std::vector<std::string>	parameters;
	std::istringstream			f( command );
	std::string					string;

	while ( getline( f, string ) )
		parameters.push_back( string );

	for (std::vector<std::string>::iterator it = parameters.begin() ; it != parameters.end() ; it++ ) {
		it->erase( it->find( '\r' ) );
		selectCommand( clientSocket, *it );
	}
	// for( int index = 3; index <= _maxSocketFd; index++ ) {// send to everyone except the listener and ourselves!
	// 	if ( FD_ISSET( index, &_master ) && index != _maxSocketFd && index != clientSocket && send( index, buffer, numberOfBytes, 0 ) == -1 ) 
	// 		perror("send");
	// }
}

