#pragma once

#include <iostream>
#include <arpa/inet.h>

class User
{
	public:
		// Constructors
		User();
		User( const User& copy );

		// Destructor
		~User();

		// Operators
		User&	operator = (const User& assiign);

		//Getters
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		int					getSocketFd() const;
		in6_addr			getIp() const;

		// Setters
		void	setNickname( const std::string& nickname );
		void	setUsername( const std::string& username );
		void	setSocketFd( const int socketFd );
		void	setIp( in6_addr ip );

		bool	userFieldsEmpty();
	
	private:
		std::string	_nickname;
		std::string	_username;
		in6_addr	_ip;
		int			_socketFd;
};
