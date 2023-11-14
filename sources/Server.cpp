#include "Server.hpp"

Server::Server() {}

Server::Server( const Server& copy )
{
	*this = copy;
}

Server::~Server() {}

Server&	Server::operator=( const Server& assign )
{
	( void )assign;

	return ( *this );
}

User	Server::getUser( const std::string& username ) { return ( _users[username] ); }

void	Server::addUser( std::string& nickname, std::string& username, int socketFd )
{
	User	user( nickname, username, socketFd );

	_users.insert( std::pair<std::string, User>( nickname, user ) );
}