#include "server.hpp"

Server::Server()
{

}

Server::Server(const Server& copy)
{
	*this = copy;
}

Server::~Server()
{

}

Server&	Server::operator=(const Server& assign)
{
	(void)assign;

	return (*this);
}