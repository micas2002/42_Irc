#include "ServerMessages.hpp"


ServerMessages::ServerMessages() {}

ServerMessages::~ServerMessages() {}

ServerMessages::ServerMessages( const ServerMessages& copy ) { *this = copy; }

ServerMessages&	ServerMessages::operator = ( const ServerMessages& assign ) {
	if ( this != &assign ) {}
	return ( *this );
}

void	ServerMessages::ERR_NOSUCHCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :No such channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_NONICKNAMEGIVEN( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 475 " + clientName  + " :No nickname given\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_NICKNAMEINUSE( const int socketFd, const std::string& clientName, const std::string& nick ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + nick + " :Nickname is already in use\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_USERNOTINCHANNEL( const int socketFd, const std::string& clientName, const std::string& nick, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + nick + " " + channelName + " :They aren't on that channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_NOTONCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :You're not on that channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_NEEDMOREPARAMS( const int socketFd, const std::string& clientName, const std::string& command ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + command + " :Not enough parameters\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_ALREADYREGISTERED( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " :You may not reregister\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_PASSWDMISMATCH( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " :Password incorrect\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_BADCHANNELKEY( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :Cannot join channel (+k)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

void	ServerMessages::ERR_CHANOPRIVSNEEDED( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :You're not channel operator\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}