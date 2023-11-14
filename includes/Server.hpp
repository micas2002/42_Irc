#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <map>

#include "User.hpp"

class Server
{
	public:
		// Constructors
		Server();
		Server( const Server& copy );

		// Destructor
		~Server();

		// Operators
		Server&	operator = ( const Server& assign );

		//Gettters
		User	getUser(const std::string& username);

		void	addUser( std::string& nickname, std::string& username, int socketFd );

	private:
		std::map<std::string, User>	_users;

};
