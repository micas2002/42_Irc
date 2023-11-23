#include "ServerMessages.hpp"


ServerMessages::ServerMessages() {}

ServerMessages::~ServerMessages() {}

ServerMessages::ServerMessages( const ServerMessages& copy ) { *this = copy; }

ServerMessages&	ServerMessages::operator = ( const ServerMessages& assign ) {
	if ( this != &assign ) {}
	return ( *this );
}


void		ServerMessages::sendError475( const int socketFd, const std::string& channelName, const std::string& clientName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :Cannot join channel (+k)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}
