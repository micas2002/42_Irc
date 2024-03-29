#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
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
#include <cerrno>

#include <map>
#include <vector>

#include "User.hpp"
#include "Channel.hpp"
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
	WHO =  468,
	NAMES = 1130,
	PART = 792,
};

enum MODES {
	MODE_I = 105,
	MODE_T = 116,
	MODE_K = 107,
	MODE_O = 111,
	MODE_L = 108
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
		
		// Close Server
		static void						closeServer( int signalnum );

		//Gettters
		User*							getUser( const std::string& username );
		User*							getUser( int socketFd );
		const std::string&				getServerPassword() const;
		const std::string&				getServerPort() const;
		Channel*						getChannel( std::string channel );

		//Setters	
		void							setServerPassword( const std::string& password );
		void							setServerPort( const std::string& port );

		void							addUser( User& user );
		void							addChannel( Channel channel );

		// Removers	
		void							removeUser( User* user );

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
		void							joinCommand( int userSocket, std::string& command );
    	void							passCommand( int userSocket, std::string& command );
		void							nickCommand( int userSocket, std::string& command );
		void							userCommand( int userSocket, std::string& command );
		void							messageComand( int userSocket, std::string& command );
		void							whoCommand( int userSocket, std::string& command );
		void							kickCommand( int userSocket, const std::string& command );
		void							quitCommand( int userSocket, std::string& command );
		void							inviteCommand( int userSocket, std::string& command );
		void							topicCommand( int userSocket, std::string& command );
		void							namesCommand( int userSocket, std::string& command );
		void							partCommand( int userSocket, std::string& command );

		// JOIN
		bool							isValidChannelName( std::string& channelName );
		void							createNewChannel( std::string& channelName, User* user );
		void							addUserToChannel( std::string& channelName, User* user , std::vector<std::string>& channelsKeys );

		// Message
		std::string						extractNick( std::string& message );

		// WHO
		void							whoChannel( int userSocket, const std::string& channelName );
		void							whoUser( int userSocket, const std::string& username );

		// MODE
		void							modeCommand( int userSocket, std::string& command );
		void							modeChannel( User* sender, std::vector< std::string > params, Channel* ch );
		void							modeMessage( User* user, const std::string& channel_name, const std::string& modes, const std::string& arguments );
		void							modeInvite( Channel* channel, std::string flag, User* sender );
		void							modeTopic( Channel* channel, std::string flag, User* sender );
		void							modeKey( Channel* channel, std::string flag, User* sender, std::string newKey );
		void							modeOperator( Channel *channel, std::string flag, User* sender, std::string receiver );
		void							modeLimit( Channel *channel, std::string flag, User* sender, std::string limit );

		// Helper functions
		void							changeNickInChannel( User* newUser, std::vector< Channel* >& regularChannels, std::vector< Channel* >& operatorChannels );

	private:
		std::map<std::string, User>		_users;
		std::map<int, std::string>		_commandInput;
		std::map<int, std::string>		_usersBySocket;
		std::map<std::string, Channel>	_channels;
		std::string						_serverPassword;
		std::string						_serverPort;

		int								_serverSocket;
		int								_maxSocketFd;
		fd_set							_master;
		fd_set							_readFds;
		struct addrinfo					_hints;

		static bool							_closeServer;
};