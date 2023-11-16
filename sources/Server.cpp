#include "Server.hpp"

Server::Server() {}

Server::Server( const Server& copy ) {
	*this = copy;
}

Server::~Server() {}

Server&	Server::operator=( const Server& assign ) {
	if ( this != &assign )
		_serverPassword = assign._serverPassword;
	return ( *this );
}

User	Server::getUser( const std::string& username ) { return ( _users[username] ); }

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

const std::string&	Server::getServerPort() const { return ( _serverPort ); }

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

void	Server::setServerPort( const std::string& port ) { _serverPort = port; }

// void	Server::addUser( std::string& nickname, std::string& username, int socketFd )
// {
// 	User	user( nickname, username, socketFd );

// 	_users.insert( std::pair<std::string, User>( nickname, user ) );
// }

bool	Server::authenticateUser( int new_fd ) {
	User		user;
    char 		buffer[1024];
    std::string input;
    int			numbytes;
	bool		passwordMatch = false;

    user.setSocketFd( new_fd );
	while ( user.userFieldsEmpty() == true || passwordMatch == false)
	{
		numbytes = recv( new_fd, buffer, sizeof( buffer ) - 1, 0 );
		if ( numbytes > 0 ) {
			buffer[numbytes] = '\0';
			char *token = strtok( buffer, "\n" );

			while ( token != NULL ) {
				// Process each line here
				input = token;

				// Check for keywords like "PASS," "NICK," "USER"
				if (checkAuthenticationCommands( input, new_fd, user, passwordMatch ) == false)
					return ( false );

				// Get the next token
				token = strtok( NULL, "\n" );
			}
		}
	}
	return ( true );
}

bool	Server::checkAuthenticationCommands( std::string& input, int new_fd, User& user, bool& passwordMatch) {
	if ( input.find( "/PASS" ) != std::string::npos ) {
		input.erase( std::remove(input.begin(), input.end(), '\''), input.end() );

		if ( checkIfPasswordsMatch( input ) == false )
		{
			std::cout << "Incorrect password! Desconecting from server..." << std::endl;
			close( new_fd );
			return ( false );
		}
		passwordMatch = true;
		std::cout << "Correct password!" << std::endl;
	}
	else if ( input.find( "/NICK" ) != std::string::npos ) {
		user.setNickname( input.substr(strlen( "/NICK " ), input.length() - strlen("/NICK ") ) ); ;
		std::cout << "Nickname added" << std::endl;
	}
	else if ( input.find( "/USER" ) != std::string::npos ) {
		user.setUsername( input.substr( strlen( "/USER " ), input.length() - strlen("/USER ") ) );
		std::cout << "Username added" << std::endl;
	}
	else
		std::cout << "Invalid command. Please use /PASS, /NICK and /USER commands" << std::endl;
	return ( true );
}

bool	Server::checkIfPasswordsMatch(const std::string& input ) const {
	int	passCommandLength = strlen( "/PASS " );

	return ( _serverPassword.compare( input.substr( passCommandLength, input.length() - passCommandLength ) ) == 0 );
}

void	Server::createAndBindSocket() {
	FD_ZERO(&_master);    // clear the master and temp sets
	FD_ZERO(&_read_fds);

	memset( &_hints, 0, sizeof( _hints ) );
	_hints.ai_family = AF_INET6;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE; // use my IP

	int	addrInfoReturn;
	struct addrinfo* serverInfo;
	if ( ( addrInfoReturn = getaddrinfo( NULL, _serverPort.c_str(), &_hints, &serverInfo ) ) != 0 )
		throw std::runtime_error( gai_strerror( addrInfoReturn ) );

	// loop through all the results and bind to the first we can
	int yes = 1; // used to tell setsockopt(  ) that the flag SO_REUSEADDR should be enabled
	struct addrinfo*	addrPointer;
	for( addrPointer = serverInfo; addrPointer != NULL; addrPointer = addrPointer->ai_next ) {
		if ( ( _serverSocket = socket( addrPointer->ai_family, addrPointer->ai_socktype, addrPointer->ai_protocol ) ) == -1 ) {
			std::perror( "server: socket" );
			continue;
		}

		if ( setsockopt( _serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
			std::perror( "setsockopt" );
			exit( 1 );
		}

		if ( bind( _serverSocket, addrPointer->ai_addr, addrPointer->ai_addrlen ) == -1 ) {
			close( _serverSocket );
			std::perror( "server: bind" );
			continue;
		}
		break;
	}
	freeaddrinfo( serverInfo );

	if ( addrPointer == NULL )
		throw std::runtime_error( "server: failed to bind" );

	if ( listen( _serverSocket, 0 ) == -1 )
		throw std::runtime_error( "failed to listen" );
			
	FD_SET( _serverSocket, &_master );
	// keep track of the biggest file descriptor
	_maxSocketFd = _serverSocket; // so far, it's this one
		std::cout << "server: awaiting connections" << std::endl;
	serverLoop();
}

void	Server::serverLoop() {
	while( 1 ) {
		_read_fds = _master; // copy read_fds into master
		if ( select( _maxSocketFd + 1, &_read_fds, NULL, NULL, NULL ) == -1 ) {
			perror("select");
			exit(4);
		}

		for( int index = 3; index <= _maxSocketFd; index++ ) {
			if ( FD_ISSET( index, &_read_fds ) )// we found one fd that is ready to be read
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
	socklen_t			sin_size;
	int					new_fd;
	struct sockaddr_in6 their_addr;
	char 				ip6[INET6_ADDRSTRLEN];

	sin_size = sizeof( struct sockaddr_storage );
	new_fd = accept( _serverSocket, ( struct sockaddr * )&their_addr, &sin_size );

	if ( new_fd == -1 )
		perror( "accept" );
	else
	{
		std::cout << "Welcome to our IRC server. Please input username, nickname and server password" << std::endl;
		std::cout << "Use /NICK, /USER and /PASS commands" << std::endl;

		if ( authenticateUser( new_fd ) ) // gets user information, like username, password, etc
		{
			FD_SET( new_fd, &_master ); // add to master set

			if ( new_fd > _maxSocketFd )
				_maxSocketFd = new_fd;
			
			inet_ntop( AF_INET6, &( their_addr.sin6_addr ), ip6, INET6_ADDRSTRLEN );
			std::cout << "server: got connection from " << ip6 << std::endl;
		}

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
			perror("recv");
		close( clientSocket ); // close the fd!
		FD_CLR( clientSocket, &_master ); // remove fd from master set
		return ;
	}
	// we got some data from a client
	for( int index = 3; index <= _maxSocketFd; index++ ) {// send to everyone except the listener and ourselves!
		if ( FD_ISSET( index, &_master ) && index != _maxSocketFd && index != clientSocket && send( index, buffer, numberOfBytes, 0 ) == -1 ) 
			perror("send");
	}
}