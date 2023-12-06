#pragma once

#include <iostream>
#include <sys/socket.h>
#include "User.hpp"

class ServerMessages {
	public:
		// ERRORS
		// ERR_BADCHANNELKEY 475
		static void		ERR_BADCHANNELKEY( int socketFd, const std::string& channelName, const std::string& clientName );

		// ERR_NEEDMOREPARAMS 461
		static void 	ERR_NEEDMOREPARAMS( int socketFD, const std::string& clientName, const std::string& commandName );

		// REPLIES
		// RPL_ENDOFWHO 315
		static void		RPL_ENDOFWHO( int socketFd, const std::string& clientName, const std::string& mask );

		// RPL_WHOREPLY 352
		static void		RPL_WHOREPLY( int socketFd, User* user,  const std::string& sender, const std::string& target );


	private:
		ServerMessages();
		~ServerMessages();
		ServerMessages( const ServerMessages& copy );
		ServerMessages&	operator = ( const ServerMessages& assign );
};