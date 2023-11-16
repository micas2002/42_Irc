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
#include <algorithm>

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
		User				getUser(const std::string& username);
		const std::string&	getServerPassword() const;
		const std::string&	getServerPort() const;

		//Setters
		void	setServerPassword( const std::string& password );
		void	setServerPort( const std::string& port );

		// void	addUser( std::string& nickname, std::string& username, int socketFd );
		bool	authenticateUser( int new_fd );
		bool	checkAuthenticationCommands( std::string& input, int new_fd, User user);
		bool	checkIfPasswordsMatch( const std::string& input ) const;

		// Sockets
		void	createAndBindSocket();
		void	serverLoop();
		void	handleNewConnection();
		void	handleClientData( int clientSocket );

	private:
		std::map<std::string, User>	_users;
		std::string					_serverPassword;
		std::string					_serverPort;

		int							_serverSocket;
		int							_maxSocketFd;
		fd_set						_master;
		fd_set						_read_fds;
		struct addrinfo				_hints;
};
