#include "ServerMessages.hpp"


ServerMessages::ServerMessages() {}

ServerMessages::~ServerMessages() {}

ServerMessages::ServerMessages( const ServerMessages& copy ) { *this = copy; }

ServerMessages&	ServerMessages::operator = ( const ServerMessages& assign ) {
	if ( this != &assign ) {}
	return ( *this );
}

// ERRORS
// ERR_NOSUCHNICK 401
void		ServerMessages::ERR_NOSUCHNICK( const int socketFd, const std::string& clientName, const std::string& nickname ) {
	std::string message( " Tijolinhos 401 " + clientName + " " + nickname + " :No such nick/channel\r\n" );

	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NOSUCHCHANNEL 403
void	ServerMessages::ERR_NOSUCHCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 403 " + clientName + " " + channelName + " :No such channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NOTEXTTOSEND 412
void		ServerMessages::ERR_NOTEXTTOSEND( const int socketFd, const std::string& clientName ) {
	std::string message ( "Tijolinhos 412 " + clientName + " :No text to send\r\n");

	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NONICKNAMEGIVEN 431
void	ServerMessages::ERR_NONICKNAMEGIVEN( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 431 " + clientName  + " :No nickname given\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NICKNAMEINUSE 433
void	ServerMessages::ERR_NICKNAMEINUSE( const int socketFd, const std::string& clientName, const std::string& nick ) {
	std::string	message( "Tijolinhos 433 " + clientName + " " + nick + " :Nickname is already in use\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_USERNOTINCHANNEL 441
void	ServerMessages::ERR_USERNOTINCHANNEL( const int socketFd, const std::string& clientName, const std::string& nick, const std::string& channelName ) {
	std::string	message( "Tijolinhos 441 " + clientName + " " + nick + " " + channelName + " :They aren't on that channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NOTONCHANNEL 442
void	ServerMessages::ERR_NOTONCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 442 " + clientName + " " + channelName + " :You're not on that channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_USERONCHANNEL 443
void	ServerMessages::ERR_USERONCHANNEL( const int socketFd, const std::string& clientName, const std::string& nick, const std::string& channelName ) {
	std::string	message( "Tijolinhos 443 " + clientName + " " + nick + " " + channelName + " :is already on channel\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_NOTREGISTERED 451
void	ServerMessages::ERR_NOTREGISTERED( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 451 " + clientName + " :You have not registered\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}
// ERR_NEEDMOREPARAMS 461
void	ServerMessages::ERR_NEEDMOREPARAMS( const int socketFd, const std::string& clientName, const std::string& command ) {
	std::string	message( "Tijolinhos 461 " + clientName + " " + command + " :Not enough parameters\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_ALREADYREGISTERED 462
void	ServerMessages::ERR_ALREADYREGISTERED( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 462 " + clientName + " :You may not reregister\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_PASSWDMISMATCH 464
void	ServerMessages::ERR_PASSWDMISMATCH( const int socketFd, const std::string& clientName ) {
	std::string	message( "Tijolinhos 464 " + clientName + " :Password incorrect\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

//ERR_CHANNELISFULL 471
void	ServerMessages::ERR_CHANNELISFULL( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 471 " + clientName + " " + channelName + " :Cannot join channel (+l)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_INVITEONLYCHAN 473
void	ServerMessages::ERR_INVITEONLYCHAN( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 473 " + clientName + " " + channelName + " :Cannot join channel (+i)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_BADCHANNELKEY 475
void	ServerMessages::ERR_BADCHANNELKEY( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 475 " + clientName + " " + channelName + " :Cannot join channel (+k)\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_BADCHANMASK 476
void	ServerMessages::ERR_BADCHANMASK( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string message( "Tijolinhos 476 " + clientName + " " + channelName + " :Bad Channel Mask\r\n" );

	send( socketFd, message.c_str(), message.size(), 0 );
}

// ERR_CHANOPRIVSNEEDED 482
void	ServerMessages::ERR_CHANOPRIVSNEEDED( const int socketFd, const std::string& clientName, const std::string& channelName ) {
	std::string	message( "Tijolinhos 482 " + clientName + " " + channelName + " :You're not channel operator\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// REPLIES
// RPL_ENDOFWHO 315
void	ServerMessages::RPL_ENDOFWHO( int socketFd, const std::string& clientName, const std::string& mask ) {
	std::string	message( "Tijolinhos 315 " + clientName + " " + mask + " :End of WHO list\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}

// RPL_WHOREPLY 352
void	ServerMessages::RPL_WHOREPLY( int socketFd, User* user, const std::string& sender, const std::string& target ) {
	std::string message( "Tijolinhos 352 " + sender + " " + target + " " + user->getUsername() \
		+ " " + user->getIp() + " Tijolinhos " + user->getNickname() + " H :0 realname\r\n" );

  send( socketFd, message.c_str(), message.size(), 0 );
}


// JOIN_MESSAGE
void	ServerMessages::JOIN_MESSAGE( const int socketFd, User* user, const std::string& channelName ) {
	std::string	message( ":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getIp()\
		+ " JOIN " + channelName + "\r\n" );
	
	send( socketFd, message.c_str(), message.size(), 0 );
}