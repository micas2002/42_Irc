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
#include <set>

#include "User.hpp"

class User;

class Channel {
	public:
		// Constructors
		Channel();
		Channel( std::string name );
		Channel( const Channel& copy );

		// Destructor
		~Channel();

		// Operators
		Channel&								operator = ( const Channel& assign );

		//Gettters
		const std::string&						getPassword() const;
		const std::string&						getName() const;
		const std::string&						getTopic() const;
		std::string								getModes() const;
  		User*									getUser( const std::string& nickname );
		std::map<std::string, User*>&			getUsers();
		std::set<std::string>&					getOperators();
		std::set<std::string>&					getInvites();
		int										getUserLimit() const;
		int										getUserCount() const;
		bool									getInviteOnly() const;
		bool									getTopicRestriction() const;
		bool									isInvited( const std::string& user ) const;
		bool									isUser( const std::string& user ) const;
		bool									isOperator( const std::string& user ) const;	

		//Setters
		void									setPassword( std::string password );
		void									setTopic( std::string topic );
		void									setUserLimit( int userLimit );
		void									setInviteOnly( bool inviteOnly );
		void									setTopicRestriction( bool topicRestriction );
		void									addUser( User* newUser );
		void									addOperator( const std::string& newOperator );
		void									addInvite( const std::string& newInvite );

		//Removers
		void									ejectUser( const std::string& user);
		void									ejectOperator( const std::string& user);
		void									removeInvite( const std::string& invite);

		//Functions
		void									sendMessage( std::string serverMessage, std::string senderNick );

	private:
		const std::string						_name;
		std::string								_channelPassword;
		std::string								_topic;
		int										_userLimit;
		bool									_inviteOnly;
		bool									_topicRestriction;

		std::map<std::string, User*>			_users;
		std::set<std::string>					_operators;
		std::set<std::string>					_inviteList;
};