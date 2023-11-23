#include "Server.hpp"

Server::Server() {}

Server::Server( const Server& copy ) {
	*this = copy;
}

Server::~Server() {}

Server&	Server::operator=( const Server& assign ) {
	if ( this != &assign ) {
		_users = assign._users;
		_serverPassword = assign._serverPassword;
		_serverPort = assign._serverPort;
		_serverSocket = assign._serverSocket;
		_maxSocketFd = assign._maxSocketFd;
		_master = assign._master;
		_readFds = assign._readFds;
		_hints = assign._hints;
	}
	return ( *this );
}

User*	Server::getUser( const std::string& nickname ) {
	std::map< std::string, User >::iterator it = _users.find( nickname );
	if ( it != _users.end() )
		return ( &(it->second) );
	return ( NULL );
}

User*	Server::getUser( int socketFd ){
	std::map< int, User& >::iterator it = _usersBySocket.find( socketFd );
	if ( it != _usersBySocket.end() )
		return ( &(it->second) );
	return ( NULL );
}

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

const std::string&	Server::getServerPort() const { return ( _serverPort ); }

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

void	Server::setServerPort( const std::string& port ) { _serverPort = port; }

void	Server::addUser( User& user ) {
	_users.insert( std::pair<std::string, User>( user.getNickname(), user ) );

	User& user2 = _users.find( user.getNickname() )->second;
	_usersBySocket.insert( std::pair<int, User&>( user2.getSocketFd(),  user2 ) );
}

void	Server::addChannel( Channel& channel ) {
	_channels.insert( std::pair<std::string, Channel>( channel.getName(), channel ) );
}

long	Server::simpleHash( std::string& command ) {
	std::string::iterator	iter;
	long					hash = 0;

	iter = command.begin();
	for ( int index = 0; iter != command.end() && *iter != ' '; ++iter, ++index ) {
		hash += ( index + 1 ) * *iter;
	}
	return ( hash );
}

void	Server::selectCommand( int userSocket, std::string& command ) {
	switch ( simpleHash( command ) ) {
		case QUIT:
			break;
		
		case JOIN:
			joinCommand( userSocket, command);
			break;
		
		case KICK:
			break;
		
		case INVITE:
			break;

		case TOPIC:
			break;
		
		case MODE:
			break;

		case PASS:
			break;

		case NICK:
			break;

		case USER:
			break;

		case PRIVMSG:
			messageComand( userSocket, command );
			break;
		
		default:
			std::cout << simpleHash( command ) << std::endl;
			break;
	}
}

std::vector<std::string>	Server::getParameters( std::string& command ) {
	std::vector<std::string>	parameters;
	std::istringstream			commandStream( command );
	std::string					tempString;

	while ( getline( commandStream, tempString, ' ') )
		parameters.push_back( tempString );
	
	return ( parameters );
}

void	Server::joinCommand( int userSocket, std::string& command ) {
	std::vector<std::string>	parameters;
	std::vector<std::string>	channelsKeys;
	std::string					channelName;
	std::string					channelKey;
	User*						user = getUser( userSocket );

	parameters = getParameters( command );
	if ( parameters.size() < 2 ) {
		// ERR_NEEDMOREPARAMS 461
		return ;
	}
	if ( parameters.size() == 3 ) {
		std::istringstream	channelKeyStream( parameters.at( 2 ) );

		while ( getline( channelKeyStream, channelKey, ',' ) ) {
			channelsKeys.push_back( channelKey );
		}
	}

	std::istringstream			channelsStream( parameters.at( 1 ) );

	while ( getline( channelsStream, channelName, ',' ) ) {
		if ( isValidChannelName( channelName ) == false ) {
			// ERR_BADCHANMASK 476
			continue ;
		}

		if ( _channels.find( channelName ) == _channels.end() ) 
			createNewChannel( channelName, user );
		else 
			addUserToChannel( channelName, user, channelsKeys );
	}
}

bool	Server::isValidChannelName( std::string& channelName ) {
	return ( channelName[0] == '#' && channelName.length() <= 51 && \
		channelName.find( ' ' ) == std::string::npos && channelName.find( ',' ) == std::string::npos );
}

void	Server::createNewChannel( std::string& channelName, User* user ) {
	Channel channel( channelName );
	
	channel.addUser( user );
	channel.addOperator( user );
	addChannel( channel );

	// send JOIN message to user
	std::string joinMessage( user->getMessagePrefix() + "JOIN " + channelName + "\r\n" );
	
	send ( user->getSocketFd(), joinMessage.c_str(), joinMessage.length(), 0 );
}

void	Server::addUserToChannel( std::string& channelName, User* user, std::vector<std::string>& channelsKeys ) {
	Channel&		channel = _channels[channelName];
	std::string&	key = channelsKeys.at( 0 );
	const std::string&	channelPassword = channel.getPassword();

	if ( channelPassword.empty() == false ) {
		if ( key != channelPassword ) {
			// ERR_BADCHANNELKEY 475
			return ;
		}
	}
	if ( channel.getUserLimit() != -1 && channel.getUserCount() >= channel.getUserLimit() ) {
		// ERR_CHANNELISFULL 471
		return ;
	}
	if ( channel.getInviteOnly() == true) {
		if ( channel.isInvited( user ) == false ) {
			// ERR_INVITEONLYCHAN 473
			return ;
		}
	}
	
	if ( channelPassword.empty() == false )
		channelsKeys.erase( channelsKeys.begin() );
	channel.removeInvite( user );
	channel.addUser( user );

	// send JOIN message to user
	std::string joinMessage( user->getMessagePrefix() + "JOIN " + channelName + "\r\n" );
	
	send ( user->getSocketFd(), joinMessage.c_str(), joinMessage.length(), 0 );
}

void	Server::messageComand( int userSocket, std::string& command )
{
	User*				sender;
	User*				recipient;
	std::string			message;

	message = command.substr( command.find( ':' ) + 1 );
	sender = getUser( userSocket );
	recipient = getUser( extractNick( command ) );
	if ( recipient == NULL ) {
		// Implement error msg
		std::cout << "Recipient error" << std::endl;
		return ;
	}
	if ( message.length() == 0)  {
		//implement error msg
		std::cout << "Message error" << std::endl;
		return ;
	}

	std::string	server_message( ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + "localhost" + " PRIVMSG " + recipient->getNickname() + " :" + message );
	send( recipient->getSocketFd(), server_message.c_str(), server_message.length(), 0);
}

std::string	Server::extractNick( std::string& message )
{
	std::string::size_type	start = message.find(' ');
	if (start != std::string::npos)
	{
		start += 1;
		std::string::size_type	end = message.find(' ', start);
		if (end != std::string::npos)
			return ( (message.substr( start, end - start ) + '\r') );
	}
	return "";
}



