#pragma once

#include <iostream>
#include <sys/socket.h>

// Colors
#define RESET   "\033[0m"
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */

class ServerMessages {
	public:
		// ERR_BADCHANNELKEY 475
		static void		sendError475( const int socketFd, const std::string& channelName, const std::string& clientName );


	private:
		ServerMessages();
		~ServerMessages();
		ServerMessages( const ServerMessages& copy );
		ServerMessages&	operator = ( const ServerMessages& assign );
};