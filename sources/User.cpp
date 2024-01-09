#include "User.hpp"

User::User() :_nickname( "InitialNickname" ), _username( "" ) {
	_passwordStatus = false;
	_usernameStatus = false;
	_nicknameStatus = false;
	_isAuthenticated = false;
}

User::User( std::string username ) :_nickname( username ), _username( "" ) {
	_passwordStatus = false;
	_usernameStatus = false;
	_nicknameStatus = false;
	_isAuthenticated = false;
}

User::User( const User& copy ) {
	_nickname = copy._nickname;
	_username = copy._username;
	_socketFd = copy._socketFd;
	_ip = copy._ip;
	_passwordStatus = copy._passwordStatus;
	_usernameStatus = copy._usernameStatus;
	_nicknameStatus = copy._nicknameStatus;
	_isAuthenticated = copy._isAuthenticated;
}

/*	DO NOT TOUCH	*/
User::~User() {}

User&	User::operator=( const User& copy ) {
	if (this != &copy) {
		_nickname = copy._nickname;
		_username = copy._username;
		_socketFd = copy._socketFd;
		_ip = copy._ip;
		_passwordStatus = copy._passwordStatus;
		_usernameStatus = copy._usernameStatus;
		_nicknameStatus = copy._nicknameStatus;
		_isAuthenticated = copy._isAuthenticated;
	}
	return ( *this );
}

// Getters

std::map<std::string, Channel*>&	User::getAllChannels() { return ( _channels ); }

std::vector< Channel* >	User::getRegularChannels() {
	std::vector< Channel* > regularChannels;

	std::map<std::string, Channel*>::iterator		chanIt = _channels.begin();
	Channel*										channel;
	for (; chanIt != _channels.end(); ++chanIt ) {
		channel = chanIt->second;
		if ( channel->isOperator( _nickname ) == false )
			regularChannels.push_back( channel );
	}
	return ( regularChannels );
}

std::vector< Channel* >	User::getOperatorChannels() {
	std::vector< Channel* > operatorChannels;

	std::map<std::string, Channel*>::iterator		chanIt = _channels.begin();
	Channel*										channel;
	for (; chanIt != _channels.end(); ++chanIt ) {
		channel = chanIt->second;
		if ( channel->isOperator( _nickname ) )
			operatorChannels.push_back( channel );
	}
	return ( operatorChannels );
}

const std::string&	User::getNickname() const { return ( _nickname ); }

const std::string&	User::getUsername() const { return ( _username ); }
	
int	User::getSocketFd() const { return ( _socketFd ); }

const std::string&	User::getIp() const { return ( _ip ); }

bool	User::getPasswordStatus() const { return ( _passwordStatus ); }

bool	User::getUsernameStatus() const { return ( _usernameStatus ); }

bool	User::getNicknameStatus() const { return ( _nicknameStatus ); }

bool	User::getIsAuthenticated() const { return ( _isAuthenticated ); }

// Setters
void	User::setNickname( const std::string& nickname ) { _nickname = nickname; }

void	User::setUsername( const std::string& username ) { _username = username; }

void	User::setSocketFd( const int socketFd ) { _socketFd = socketFd; }

void	User::setIp( const std::string& ip ) { _ip = ip; }

void	User::setPasswordStatusTrue() { _passwordStatus = true; }

void	User::setUsernameStatusTrue() { _usernameStatus = true; }

void	User::setNicknameStatusTrue() { _nicknameStatus = true; }

void	User::setIsAuthenticatedTrue() { _isAuthenticated = true; }

void	User::addChannel( const std::string& channelName, Channel* channel ) {
	if ( _channels.find( channelName ) == _channels.end() )
		_channels.insert( std::pair<std::string, Channel*>( channelName, channel ) );
}

void	User::removeChannel( const std::string& channelName ) {
	if ( _channels.find( channelName ) != _channels.end() ) {
		_channels.erase( channelName );
	}
}

std::string	User::getMessagePrefix() const {
	std::string	messagePrefix( ":" + _nickname + "!" + _username + "@" + _ip + " " );
	return ( messagePrefix );
}

bool	User::userFieldsEmpty() {
	if ( _nickname.empty() == true || _username.empty() == true )
		return ( true );
	return ( false );
}
