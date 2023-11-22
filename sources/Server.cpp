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
	std::map< int, User* >::iterator it = _usersBySocket.find( socketFd );
	if ( it != _usersBySocket.end() )
	{
		std::cout << "Entering where it should" << std::endl;
		return ( it->second );
	}
	return ( NULL );
}

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

const std::string&	Server::getServerPort() const { return ( _serverPort ); }

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

void	Server::setServerPort( const std::string& port ) { _serverPort = port; }

void	Server::addUser( User& user ) {
	_users.insert( std::pair<std::string, User>( user.getNickname(), user ) );
	_usersBySocket.insert( std::pair<int, User*>( user.getSocketFd(),  &user ) );
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

void	Server::joinCommand( int userSocket, std::string& command ) {
	std::string									channelName;
	std::map<std::string, Channel>::iterator	iter;
	User*										user;

	channelName = command.substr( 5, command.length() - 5 );
	iter = _channels.find( channelName );
	user = getUser( userSocket );

	if ( iter == _channels.end() )
		createNewChannel( channelName, user );
		
}

void	Server::createNewChannel( std::string& channelName, User* user ) {
	Channel channel( channelName );
	

	channel.addUser( user );
	channel.addOperator( user );
	addChannel( channel );
}

void	Server::passCommand( int userSocket, std::string& command ) {
	// command.erase( std::remove(command.begin(), command.end(), '\''), command.end() );

	if ( checkIfPasswordsMatch( command ) == false )
	{
		std::cout << SERVER_INCORRECT_PASSWORD << std::endl;
		send( userSocket, SERVER_INCORRECT_PASSWORD,  56, 0 );
		close( userSocket );
		FD_CLR( userSocket, &_master );
		return;
	}
	send( userSocket, SERVER_CORRECT_PASSWORD, 26, 0 );
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