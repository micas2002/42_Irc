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

const std::string&	Server::getServerPassword() const { return ( _serverPassword ); }

void	Server::setServerPassword( const std::string& password) { _serverPassword = password; }

// void	Server::addUser( std::string& nickname, std::string& username, int socketFd )
// {
// 	User	user( nickname, username, socketFd );

// 	_users.insert( std::pair<std::string, User>( nickname, user ) );
// }

bool	Server::authenticateUser( int new_fd )
{
	User		user;
    char 		buf[1024];
    std::string input;
    int			numbytes;

    user.setSocketFd(new_fd);

    while ( ( numbytes = recv( new_fd, buf, sizeof( buf ) - 1, 0 ) ) > 0 && user.checkUserInfo() == false)
    {
        buf[numbytes] = '\0';
        char *token = strtok( buf, "\n" );

        while ( token != NULL )
        {
            // Process each line here
            input = token;

            // Check for keywords like "PASS," "NICK," "USER"
            // if ( input.find( "PASS" ) != std::string::npos )
            // {
			// 	input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
			// 	if ( _serverPassword.compare(input.substr( strlen( "PASS " ) ) ) != 0)
			// 	{
			// 		std::cout << "Incorrect password! Desconecting from server..." << std::endl;
			// 		close( new_fd );
			// 		return ( false );
			// 	}
			// 	std::cout << "Correct password!" << std::endl;
            // }
            if ( input.find( "NICK" ) != std::string::npos )
			{
				user.setNickname(input.substr(strlen("NICK")));
				std::cout << "Nickname added" << std::endl;
			}
            else if (input.find("USER") != std::string::npos)
			{
				user.setUsername( input.substr( strlen( "USER" ) ) );
				std::cout << "Username added" << std::endl;
			}
			else
				std::cout << "Invalid command. Please use /PASS, /NICK and /USER" << std::endl;

            // Get the next token
            token = strtok( NULL, "\n" );
        }
    }
	return ( true );
}