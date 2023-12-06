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
		static void			ERR_NOSUCHCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName );
		static void			ERR_NONICKNAMEGIVEN( const int socketFd, const std::string& clientName );
		static void			ERR_NICKNAMEINUSE( const int socketFd, const std::string& clientName, const std::string& nick );
		static void			ERR_USERNOTINCHANNEL( const int socketFd, const std::string& clientName, const std::string& nick, const std::string& channelName );
		static void			ERR_NOTONCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName );
		static void			ERR_NEEDMOREPARAMS( const int socketFd, const std::string& clientName, const std::string& command );
		static void			ERR_ALREADYREGISTERED( const int socketFd, const std::string& clientName );
		static void			ERR_PASSWDMISMATCH( const int socketFd, const std::string& clientName	);
		static void			ERR_BADCHANNELKEY( const int socketFd, const std::string& clientName, const std::string& channelName );
		static void			ERR_CHANOPRIVSNEEDED( const int socketFd, const std::string& clientName, const std::string& channelName );

	private:
		ServerMessages();
		~ServerMessages();
		ServerMessages( const ServerMessages& copy );
		ServerMessages&		operator = ( const ServerMessages& assign );
};