#pragma once

#include <iostream>
#include <sys/socket.h>
#include "User.hpp"

class ServerMessages {
	public:

		// REPLIES
		// RPL_ENDOFWHO 315
		static void		RPL_ENDOFWHO( int socketFd, const std::string& clientName, const std::string& mask );

		// RPL_WHOREPLY 352
		static void		RPL_WHOREPLY( int socketFd, User* user,  const std::string& sender, const std::string& target );

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