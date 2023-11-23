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
		bool				getPasswordStatus() const;
		bool				getUsernameStatus() const;
		bool				getNicknameStatus() const;
		bool				getUserIsAuthenticated() const;

		std::string		getMessagePrefix() const;
		char*			getIp() const;


		// Setters
		void	setNickname( const std::string& nickname );
		void	setUsername( const std::string& username );
		void	setSocketFd( const int socketFd );
		void	setPasswordStatusTrue();
		void	setUsernameStatusTrue();
		void	setNicknameStatusTrue();
		void	setUserIsAuthenticatedTrue();
		void	setIp( char* ip );

		bool	userFieldsEmpty();
	
	private:
		std::string	_nickname;
		std::string	_username;
		char*		_ip;
		int			_socketFd;
		bool		_passwordStatus;
		bool		_usernameStatus;
		bool		_nicknameStatus;
		bool		_userIsAuthenticated;
};
