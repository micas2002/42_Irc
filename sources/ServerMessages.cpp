#include "ServerMessages.hpp"


ServerMessages::ServerMessages() {}

ServerMessages::~ServerMessages() {}

ServerMessages::ServerMessages( const ServerMessages& copy ) { *this = copy; }

ServerMessages&	ServerMessages::operator = ( const ServerMessages& assign ) {
	if ( this != &assign ) {}
	return ( *this );
}

// ERRORS
void	ServerMessages::ERR_BADCHANNELKEY( int socketFd, const std::string& channelName, const std::string& clientName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :Cannot join channel (+k)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_NEEDMOREPARAMS( int socketFd, const std::string& clientName, const std::string& commandName ) {
	std::string	message( "Tijolinhos 461 " + clientName + " " + commandName + " :Not enough parameters\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// REPLIES
void	ServerMessages::RPL_ENDOFWHO( int socketFd, const std::string& clientName, const std::string& mask ) {
	std::string	message( "Tijolinhos 315 " + clientName + " " + mask + " :End of WHO list\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::RPL_WHOREPLY( int socketFd, User* user, const std::string& sender, const std::string& target ) {
	std::string message( "Tijolinhos 352 " + sender + " " + target + " " + user->getUsername() \
		+ " " + user->getIp() + " Tijolinhos " + user->getNickname() + " H :0 realname\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}