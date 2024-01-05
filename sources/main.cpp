#include "Server.hpp"

int	main( int argc, char** argv ) {
	try {
		if ( argc != 3 )
			throw std::runtime_error( "usage: ./irc <port> <password>" );

		Server	server;
		server.setServerPassword( argv[2] );
		server.setServerPort( argv[1] );
		server.createAndBindSocket();
		server.serverLoop();
	}
	catch ( std::exception &e ) {
		e.what();
	}
}
