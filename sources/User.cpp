#include "User.hpp"

User::User() : _passwordStatus( false ), _usernameStatus( false ), _nicknameStatus( false ), _userIsAuthenticated( false ) {}

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

// Getters
const std::string&	User::getNickname() const { return ( _nickname ); }

const std::string&	User::getUsername() const { return ( _username ); }
	
int	User::getSocketFd() const { return ( _socketFd ); }

bool	User::getPasswordStatus() const { return ( _passwordStatus ); }

bool	User::getUsernameStatus() const { return ( _usernameStatus ); }

bool	User::getNicknameStatus() const { return ( _nicknameStatus ); }

bool	User::getUserIsAuthenticated() const { return ( _userIsAuthenticated ); }

// Setters
void	User::setNickname( const std::string& nickname ) { _nickname = nickname; }

void	User::setUsername( const std::string& username ) { _username = username; }

void	User::setSocketFd( const int socketFd ) { _socketFd = socketFd; }

void	User::setPasswordStatusTrue() { _passwordStatus = true; }

void	User::setUsernameStatusTrue() { _usernameStatus = true; }

void	User::setNicknameStatusTrue() { _nicknameStatus = true; }

void	User::setUserIsAuthenticatedTrue() { _userIsAuthenticated = true; }

bool	User::userFieldsEmpty()
{
	if ( _nickname.empty() == true || _username.empty() == true )
		return ( true );
	else
		return ( false );
}