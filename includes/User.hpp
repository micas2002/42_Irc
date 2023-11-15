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

		// Setters
		void	setNickname( const std::string& nickname );
		void	setUsername( const std::string& username );
		void	setSocketFd( const int socketFd );

		bool	checkUserInfo();
	
	private:
		std::string	_nickname;
		std::string	_username;
		int			_socketFd;
};
