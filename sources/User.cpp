#include "User.hpp"

User::User() {}

User::User( const User& copy ) {
	*this = copy;
}

User::~User() {}

User&	User::operator=( const User& assign ) {
	if (this != &assign) {
		_nickname = assign._nickname;
		_username = assign._username;
		_socketFd = assign._socketFd;
	}
	return ( *this );
}

const std::string&	User::getNickname() const { return ( _nickname ); }

const std::string&	User::getUsername() const { return ( _username ); }

int	User::getSocketFd() const { return ( _socketFd ); }

std::string	User::getMessagePrefix() const {
	std::string	messagePrefix( ":" + _nickname + "!" + _username + "@" + "localhost " );
	return ( messagePrefix );
}

char*	User::getIp() const { return ( _ip ); }	

bool	User::userFieldsEmpty()
{
	if ( _nickname.empty() == true || _username.empty() == true )
		return ( true );
	else
		return ( false );
}

void	User::setNickname( const std::string& nickname ) { _nickname = nickname; }

void	User::setUsername( const std::string& username ) { _username = username; }

void	User::setSocketFd( const int socketFd ) { _socketFd = socketFd; }

void	User::setIp( char* new_ip ) { _ip = new_ip; }