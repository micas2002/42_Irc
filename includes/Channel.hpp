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

#include "./User.hpp"

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
		User*									getUser( const std::string& nickname );
		User*									getOperator( const std::string& nickname );
		const std::map<std::string, User*>&		getUsers() const;
		const std::map<std::string, User*>&		getOperators() const;
		const std::map<std::string, User*>&		getInvites() const;
		int										getUserLimit() const;
		int										getUserCount() const;
		bool									getInviteOnly() const;
		bool									isInvited( User* user ) const;

		//Setters
		void									setPassword( std::string password );
		void									setTopic( std::string topic );
		void									setUserLimit( int userLimit );
		void									setInviteOnly( bool inviteOnly );
		void									setTopicRestriction( bool topicRestriction );
		void									addUser( User* newUser );
		void									addOperator( User* newOperator );
		void									addInvite( User* newInvite);

		//Removers
		void									ejectUser(User* user);
		void									ejectOperator(User* user);
		void									removeInvite(User* invite);

		//Functions
		void									sendMessage( std::string serverMessage, std::string senderNick );
		bool									isOperator( User* user );

	private:
		const std::string						_name;
		std::string								_channelPassword;
		std::string								_topic;
		int										_userLimit;
		bool									_inviteOnly;
		bool									_topicRestriction;

		std::map<std::string, User*>			_users;
		std::map<std::string, User*>			_operators;
		std::map<std::string, User*>			_inviteList;
		
};