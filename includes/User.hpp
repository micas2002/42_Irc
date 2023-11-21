#pragma once

#include <iostream>

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

		// Setters
		void	setNickname( const std::string& nickname );
		void	setUsername( const std::string& username );
		void	setSocketFd( const int socketFd );
		void	setPasswordStatusTrue();
		void	setUsernameStatusTrue();
		void	setNicknameStatusTrue();
		void	setUserIsAuthenticatedTrue();

		bool	userFieldsEmpty();
	
	private:
		std::string	_nickname;
		std::string	_username;
		int			_socketFd;
		bool		_passwordStatus;
		bool		_usernameStatus;
		bool		_nicknameStatus;
		bool		_userIsAuthenticated;
};
