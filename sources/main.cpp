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

		std::cout << "server: awaiting connections" << std::endl;

		Server server;

		while( 1 )
		{
			socklen_t sin_size = sizeof( struct sockaddr_storage );

			new_fd = accept( sockfd, ( struct sockaddr * )&their_addr, &sin_size );
			if ( new_fd == -1 )
			{
				std::cout << "error\n";
				perror( "accept" );
				continue;
			}

			inet_ntop( AF_INET6, &( their_addr.sin6_addr ), ip6, INET6_ADDRSTRLEN );
			printf( "server: got connection from %s\n", ip6 );

			if ( !fork() ) // this is the child process
			{
				close( sockfd ); // child doesn't need the listener

				// Buffer to store the data received from the client
				char buf[1024];
				int numbytes;

				// Wait for a message from the client
				while ( ( numbytes = recv( new_fd, buf, sizeof( buf ) - 1, 0 ) ) > 0 )
				{
					// Null-terminate the string
					buf[numbytes] = '\0';
					// Print the message from the client
					printf( "server: received '%s'\n", buf );

					// Optionally, send a reply or perform other work...
				}

				if ( numbytes == -1 )
				{
					perror( "recv" );
				}

				close( new_fd );
				exit( 0 );
			}
			// The parent process doesn't need new_fd after forking
			close( new_fd );
		}
	}
	catch ( std::exception &e )
	{
		e.what();
	}

	return ( 0 );
}
