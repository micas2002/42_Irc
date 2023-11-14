#include "User.hpp"

User::User()  : _nickname( "default nickname" ), _username( "default username" ), _socketFd( 0 ) {}

User::User( const std::string& nickname, const std::string& username, const int socketFd ) : _nickname( nickname ), _username( username ), _socketFd( socketFd ) {}

User::User( const User& copy)
{
	*this = copy;
}

User::~User() {}

User&	User::operator=( const User& assign )
{
	( void )assign;

	return ( *this );
}

const std::string&	User::getNickname() const { return ( _nickname ); }

const std::string&	User::getUsername() const { return ( _username ); }

int	User::getSocketFd() const { return ( _socketFd ); }

void	User::setNickname( const std::string& nickname ) { _nickname = nickname; }

void	User::setUsername( const std::string& username ) { _username = username; }

void	User::setSocketFd( const int socketFd ) { _socketFd = socketFd; }