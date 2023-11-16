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

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

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

    user.setSocketFd( new_fd );
	// numbytes = recv( new_fd, buffer, sizeof( buffer ) - 1, 0 );
    while ( ( numbytes = recv( new_fd, buffer, sizeof( buffer ) - 1, 0 ) ) > 0 || user.userFieldsEmpty() == true) {
        buffer[numbytes] = '\0';
        char *token = strtok( buffer, "\n" );

        while ( token != NULL ) {
            // Process each line here
            input = token;

            // Check for keywords like "PASS," "NICK," "USER"
            if (checkAuthenticationCommands( input, new_fd, user ) == false)
				return ( false );

            // Get the next token
			free( token );
            token = strtok( NULL, "\n" );
        }
		// numbytes = recv( new_fd, buffer, sizeof( buffer ) - 1, 0 );
    }
	return ( true );
}

bool	Server::checkAuthenticationCommands( std::string& input, int new_fd, User user ) {
	if ( input.find( "/PASS" ) != std::string::npos ) {
		input.erase( std::remove(input.begin(), input.end(), '\''), input.end() );

		if ( checkIfPasswordsMatch( input ) == false )
		{
			std::cout << "Incorrect password! Desconecting from server..." << std::endl;
			close( new_fd );
			return ( false );
		}
		std::cout << "Correct password!" << std::endl;
	}
	else if ( input.find( "/NICK" ) != std::string::npos ) {
		user.setNickname( input.substr(strlen( "/NICK " ), input.length() - strlen("/NICK ") ) );
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
	// int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	// 	struct addrinfo hints, *servinfo, *p;
	// 	int rv; // for getaddrinfo error handling 
	// 	int yes = 1; // used to tell setsockopt(  ) that the flag SO_REUSEADDR should be enabled
	// 	struct sockaddr_in6 their_addr; // variable to hold any IPV4 or IPV6 address
	// 	char ip6[INET6_ADDRSTRLEN]; // variable to hold the IP in readable form
	// 	socklen_t sin_size;
	// 	fd_set master;    // master file descriptor list
	// 	fd_set read_fds;  // temp file descriptor list for select()
	// 	int fdmax; //keep track of max fd in master

	// 	char buf[256];    // buffer for client data
	// 	int nbytes;

	// 	FD_ZERO(&master);    // clear the master and temp sets
	// 	FD_ZERO(&read_fds);

	// 	memset( &hints, 0, sizeof hints );
	// 	hints.ai_family = AF_INET6;
	// 	hints.ai_socktype = SOCK_STREAM;
	// 	hints.ai_flags = AI_PASSIVE; // use my IP

	// 	if ( ( rv = getaddrinfo( NULL, argv[1], &hints, &servinfo ) ) != 0 )
	// 		throw std::runtime_error( gai_strerror( rv ) );
	
	// 	// loop through all the results and bind to the first we can
	// 	for( p = servinfo; p != NULL; p = p->ai_next )
	// 	{
	// 		if ( ( sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 )
	// 		{
	// 			std::perror( "server: socket" );
	// 			continue;
	// 		}

	// 		if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) 
	// 		{
	// 			std::perror( "setsockopt" );
	// 			exit( 1 );
	// 		}

	// 		if ( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) 
	// 		{
	// 			close( sockfd );
	// 			std::perror( "server: bind" );
	// 			continue;
	// 		}
	// 		break;
	// 	}

	// 	freeaddrinfo( servinfo );

	// 	if ( p == NULL )
	// 		throw std::runtime_error( "server: failed to bind" );

	// 	if ( listen( sockfd, 0 ) == -1 )
	// 		throw std::runtime_error( "failed to listen" );
	
			
	// 	FD_SET(sockfd, &master);
	// 	// keep track of the biggest file descriptor
	// 	fdmax = sockfd; // so far, it's this one

	// 	std::cout << "server: awaiting connections" << std::endl;
}

void	Server::serverLoop() {
	// while( 1 ) {
	// 	read_fds = master; // copy read_fds into master
	// 	if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
	// 		perror("select");
	// 		exit(4);
	// 	}

	// 	for(int i = 0; i <= fdmax; i++) {
	// 		if (FD_ISSET(i, &read_fds))// we found one fd that is ready to be read
	// 		{
	// 			if (i == sockfd) // new user trying to connect
	// 				handleNewConnection();
	// 			else 
	// 				handleClientData();
	// 		}
	// 	}
	// }
}

void	Server::handleNewConnection() {
	// handle new connections
	// sin_size = sizeof( struct sockaddr_storage );
	// new_fd = accept( sockfd, ( struct sockaddr * )&their_addr, &sin_size );

	// if ( new_fd == -1 )
	// 	perror( "accept" );
	// else
	// {
	// 	std::cout << "Welcome to our IRC server. Please input username, nickname and server password" << std::endl;
	// 	std::cout << "Use /NICK, /USER and /PASS commands" << std::endl;

	// 	if ( server.authenticateUser( new_fd ) ) // gets user information, like username, password, etc
	// 	{
	// 		FD_SET( new_fd, &master ); // add to master set

	// 		if (new_fd > fdmax)
	// 			fdmax = new_fd;
			
	// 		inet_ntop( AF_INET6, &( their_addr.sin6_addr ), ip6, INET6_ADDRSTRLEN );
	// 		std::cout << "server: got connection from " << ip6 << std::endl;
	// 	}

	// }
}

void	Server::handleClientData() {
// // 	// handle data from a client
// // 	if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {// got error or connection closed by client
// // 		if (nbytes == 0) // connection closed
// // 			printf("selectserver: socket %d hung up\n", i);
// // 		else
// // 			perror("recv");
// // 		close(i); // close the fd!
// // 		FD_CLR(i, &master); // remove fd from master set
// // 	}
// // 	else {// we got some data from a client
// // 		for(int j = 0; j <= fdmax; j++) {// send to everyone except the listener and ourselves!
// // 			if ( FD_ISSET( j, &master ) && j != sockfd && j != i && send( j, buf, nbytes, 0 ) == -1 ) 
// // 				perror("send");
// // 		}
// // 	}
}