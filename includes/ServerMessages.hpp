#pragma once

#include <iostream>
#include <sys/socket.h>
#include "User.hpp"

class ServerMessages {
	public:
		// ERROR
		// ERR_NOSUCHCHANNEL 403
		static void		ERR_NOSUCHCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_NONICKNAMEGIVEN 431
		static void		ERR_NONICKNAMEGIVEN( const int socketFd, const std::string& clientName );
		// ERR_NICKNAMEINUSE 433
		static void		ERR_NICKNAMEINUSE( const int socketFd, const std::string& clientName, const std::string& nick );
		// ERR_USERNOTINCHANNEL 441
		static void		ERR_USERNOTINCHANNEL( const int socketFd, const std::string& clientName, const std::string& nick, const std::string& channelName );
		// ERR_NOTONCHANNEL 442
		static void		ERR_NOTONCHANNEL( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_NEEDMOREPARAMS 461
		static void		ERR_NEEDMOREPARAMS( const int socketFd, const std::string& clientName, const std::string& command );
		// ERR_ALREADYREGISTERED 462
		static void		ERR_ALREADYREGISTERED( const int socketFd, const std::string& clientName );
		// ERR_PASSWDMISMATCH 464
		static void		ERR_PASSWDMISMATCH( const int socketFd, const std::string& clientName );
		//ERR_CHANNELISFULL 471
		static void 	ERR_CHANNELISFULL( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_INVITEONLYCHAN 473
		static void		ERR_INVITEONLYCHAN( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_BADCHANNELKEY 475
		static void		ERR_BADCHANNELKEY( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_BADCHANMASK 476
		static void		ERR_BADCHANMASK( const int socketFd, const std::string& clientName, const std::string& channelName );
		// ERR_CHANOPRIVSNEEDED 482
		static void		ERR_CHANOPRIVSNEEDED( const int socketFd, const std::string& clientName, const std::string& channelName );

		// REPLIES
		// RPL_ENDOFWHO 315
		static void		RPL_ENDOFWHO( int socketFd, const std::string& clientName, const std::string& mask );
		// RPL_WHOREPLY 352
		static void		RPL_WHOREPLY( int socketFd, User* user,  const std::string& sender, const std::string& target );
		// RPL_USER_MODES
		static void		RPL_USER_MODES( int socketFd, User* user, const std::string& channel_name, const std::string& modes );

	private:
		ServerMessages();
		~ServerMessages();
		ServerMessages( const ServerMessages& copy );
		ServerMessages&		operator = ( const ServerMessages& assign );
};