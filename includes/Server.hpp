#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <algorithm>
#include <sstream>

#include <map>
#include <vector>

#include "User.hpp"
#include "Channel.hpp"
#include "Messages.hpp"
#include "ServerMessages.hpp"

enum COMMANDS {
	QUIT = 806,
	JOIN = 763,
	KICK = 722,
	INVITE = 1613,
	TOPIC = 1109,
	MODE = 715,
	PASS =  791,
	NICK = 725,
	USER = 786,
	PRIVMSG = 2187,
};

class Server
{
	public:
		// Constructors
		Server();
		Server( const Server& copy );

		// Destructor
		~Server();

		// Operators
		Server&							operator = ( const Server& assign );

		//Gettters
		User*							getUser( const std::string& username );
		User*							getUser( int socketFd );
		const std::string&				getServerPassword() const;
		const std::string&				getServerPort() const;
		Channel*						getChannel( std::string channel );

		//Setters	
		void							setServerPassword( const std::string& password );
		void							setServerPort( const std::string& port );

		void							addUser( User user );
		void							addChannel( Channel& channel );

		// Removers	
		void							removeUser( User& user );

		std::vector<std::string>		splitByCharacter( const std::string& input, char c );

		// Simple Hash Function	
		long							simpleHash( std::string& command );

		bool							checkIfPasswordsMatch( const std::string& input ) const;
		bool							findDuplicateNicknames( const std::string& nickname ) const;

		// Sockets	
		void							createAndBindSocket();
		void							serverLoop();
		void							handleNewConnection();
		void							handleClientData( int clientSocket );

		// Commands	
		void							selectCommand( int userSocket, std::string& command );

		// JOIN	
		void							joinCommand( int userSocket, std::string& command );
		bool							isValidChannelName( std::string& channelName );
		void							createNewChannel( std::string& channelName, User* user );
		void							addUserToChannel( std::string& channelName, User* user , std::vector<std::string>& channelsKeys );

		void							messageComand( int userSocket, std::string& command );
		std::string						extractNick( std::string& message );
	
    	void							passCommand( int userSocket, std::string& command );
		void							nickCommand( int userSocket, std::string& command );
		void							userCommand( int userSocket, std::string& command );

	private:
		std::map<std::string, User>		_users;
		std::map<int, std::string>		_usersBySocket;
		std::map<std::string, Channel>	_channels;
		std::string						_serverPassword;
		std::string						_serverPort;

		int								_serverSocket;
		int								_maxSocketFd;
		fd_set							_master;
		fd_set							_readFds;
		struct addrinfo					_hints;
};