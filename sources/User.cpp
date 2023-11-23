#include "User.hpp"

User::User() :_nickname( "InitialNickname" ), _username( "" ) {
	_passwordStatus = false;
	_usernameStatus = false;
	_nicknameStatus = false;
	_isAuthenticated = false;
}

User::User(std::string username) :_nickname( username ), _username( "" ) {}

User::User( const User& copy ) {
	_nickname = copy._nickname;
	_username = copy._username;
	_socketFd = copy._socketFd;
	_passwordStatus = copy._passwordStatus;
	_usernameStatus = copy._usernameStatus;
	_nicknameStatus = copy._nicknameStatus;
	_isAuthenticated = copy._isAuthenticated;
}

User::~User() {}

User&	User::operator=( const User& copy ) {
	if (this != &copy) {
		_nickname = copy._nickname;
		_username = copy._username;
		_socketFd = copy._socketFd;
		_passwordStatus = copy._passwordStatus;
		_usernameStatus = copy._usernameStatus;
		_nicknameStatus = copy._nicknameStatus;
		_isAuthenticated = copy._isAuthenticated;
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

bool	User::getIsAuthenticated() const { return ( _isAuthenticated ); }

// Setters
void	User::setNickname( const std::string& nickname ) { _nickname = nickname; }

void	User::setUsername( const std::string& username ) { _username = username; }

void	User::setSocketFd( const int socketFd ) { _socketFd = socketFd; }

void	User::setPasswordStatusTrue() { _passwordStatus = true; }

void	User::setUsernameStatusTrue() { _usernameStatus = true; }

void	User::setNicknameStatusTrue() { _nicknameStatus = true; }

void	User::setIsAuthenticatedTrue() { _isAuthenticated = true; }

std::string	User::getMessagePrefix() const {
	std::string	messagePrefix( ":" + _nickname + "!" + _username + "@" + "localhost " );
	return ( messagePrefix );
}

bool	User::userFieldsEmpty() {
	if ( _nickname.empty() == true || _username.empty() == true )
		return ( true );
	return ( false );
}
