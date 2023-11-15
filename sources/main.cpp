#include "Server.hpp"

int	main( int argc, char **argv )
{
	try
	{	
		if ( argc != 3 )
			throw std::runtime_error( "usage: ./irc <port> <password>" );

		int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
		struct addrinfo hints, *servinfo, *p;
		int rv; // for getaddrinfo error handling 
		int yes = 1; // used to tell setsockopt(  ) that the flag SO_REUSEADDR should be enabled
		struct sockaddr_in6 their_addr; // variable to hold any IPV4 or IPV6 address
		char ip6[INET6_ADDRSTRLEN]; // variable to hold the IP in readable form
		socklen_t sin_size;
		fd_set master;    // master file descriptor list
		fd_set read_fds;  // temp file descriptor list for select()
		int fdmax; //keep track of max fd in master

		char buf[256];    // buffer for client data
		int nbytes;

		FD_ZERO(&master);    // clear the master and temp sets
		FD_ZERO(&read_fds);

		memset( &hints, 0, sizeof hints );
		hints.ai_family = AF_INET6;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // use my IP

		if ( ( rv = getaddrinfo( NULL, argv[1], &hints, &servinfo ) ) != 0 )
			throw std::runtime_error( gai_strerror( rv ) );
	
		// loop through all the results and bind to the first we can
		for( p = servinfo; p != NULL; p = p->ai_next )
		{
			if ( ( sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 )
			{
				std::perror( "server: socket" );
				continue;
			}

			if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) 
			{
				std::perror( "setsockopt" );
				exit( 1 );
			}

			if ( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) 
			{
				close( sockfd );
				std::perror( "server: bind" );
				continue;
			}
			break;
		}

		freeaddrinfo( servinfo );

		if ( p == NULL )
			throw std::runtime_error( "server: failed to bind" );

		if ( listen( sockfd, 0 ) == -1 )
			throw std::runtime_error( "failed to listen" );
	
			
		FD_SET(sockfd, &master);
		// keep track of the biggest file descriptor
		fdmax = sockfd; // so far, it's this one

		std::cout << "server: awaiting connections" << std::endl;

		Server server;

		server.setServerPassword(argv[2]);

		while( 1 )
		{
			read_fds = master; // copy read_fds into master
			if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
				perror("select");
				exit(4);
			}

			for(int i = 0; i <= fdmax; i++)
			{
				if (FD_ISSET(i, &read_fds))// we found one fd that is ready to be read
				{
					if (i == sockfd) // new user trying to connect
					{
						// handle new connections
						sin_size = sizeof( struct sockaddr_storage );
						new_fd = accept( sockfd, ( struct sockaddr * )&their_addr, &sin_size );

						if ( new_fd == -1 )
							perror( "accept" );
						else
						{
							std::cout << "Welcome to our IRC server. Please input username, nickname and server password" << std::endl;
							std::cout << "Use /NICK, /USER and /PASS commands" << std::endl;

							if (server.authenticateUser( new_fd )) // gets user information, like username, password, etc
							{
								FD_SET( new_fd, &master ); // add to master set

								if (new_fd > fdmax)
									fdmax = new_fd;
								
								inet_ntop( AF_INET6, &( their_addr.sin6_addr ), ip6, INET6_ADDRSTRLEN );
								std::cout << "server: got connection from " << ip6 << std::endl;
							}

						}
					}
					else
					{
						// handle data from a client
						if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) // got error or connection closed by client
						{
							if (nbytes == 0) // connection closed
								printf("selectserver: socket %d hung up\n", i);
							else
								perror("recv");
							close(i); // close the fd!
							FD_CLR(i, &master); // remove fd from master set
						}
						else // we got some data from a client
						{
							for(int j = 0; j <= fdmax; j++) // send to everyone except the listener and ourselves!
							{
								if (FD_ISSET(j, &master)) {
									if (j != sockfd && j != i) {
										if (send(j, buf, nbytes, 0) == -1) {
											perror("send");
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	catch ( std::exception &e )
	{
		e.what();
}

	return ( 0 );
}
