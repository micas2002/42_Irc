#pragma once

#include <iostream>
#include <arpa/inet.h>

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
		User&				operator = (const User& assiign);

		//Getters
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		int					getSocketFd() const;
		bool				getPasswordStatus() const;
		bool				getUsernameStatus() const;
		bool				getNicknameStatus() const;
		bool				getIsAuthenticated() const;

		std::string			getMessagePrefix() const;

		// Setters
		void				setNickname( const std::string& nickname );
		void				setUsername( const std::string& username );
		void				setSocketFd( const int socketFd );
		void				setPasswordStatusTrue();
		void				setUsernameStatusTrue();
		void				setNicknameStatusTrue();
		void				setIsAuthenticatedTrue();

		bool				userFieldsEmpty();
	
	private:
		std::string			_nickname;
		std::string			_username;
		int					_socketFd;

		bool				_passwordStatus;
		bool				_usernameStatus;
		bool				_nicknameStatus;
		bool				_isAuthenticated;
};
