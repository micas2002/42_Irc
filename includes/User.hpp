#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include "Channel.hpp"

class Channel;

class User
{
	public:
		// Constructors
		User();
		User(std::string);
		User( const User& copy );

		// Destructor
		~User();

		// Operators
		User&								operator = (const User& assiign);

		//Getters
		std::map<std::string, Channel*>&	getAllChannels();
		std::vector< Channel* >			getRegularChannels();
		std::vector< Channel* >			getOperatorChannels();
		const std::string&					getNickname() const;
		const std::string&					getUsername() const;
		int									getSocketFd() const;
		const std::string&					getIp() const;
		bool								getPasswordStatus() const;
		bool								getUsernameStatus() const;
		bool								getNicknameStatus() const;
		bool								getIsAuthenticated() const;

		std::string							getMessagePrefix() const;

		// Setters
		void								setNickname( const std::string& nickname );
		void								setUsername( const std::string& username );
		void								setSocketFd( const int socketFd );
		void								setIp( const std::string& ip );
		void								setPasswordStatusTrue();
		void								setUsernameStatusTrue();
		void								setNicknameStatusTrue();
		void								setIsAuthenticatedTrue();

		void								addChannel( const std::string& channelName, Channel* channel );
		void								removeChannel( const std::string& channelName );

		bool								userFieldsEmpty();
	
	private:
		std::map<std::string, Channel*>		_channels;
		std::string							_nickname;
		std::string							_username;
		int									_socketFd;
		std::string							_ip;

		bool								_passwordStatus;
		bool								_usernameStatus;
		bool								_nicknameStatus;
		bool								_isAuthenticated;
};
