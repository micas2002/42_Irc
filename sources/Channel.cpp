#include "../includes/Channel.hpp"

Channel::Channel() : _name( "" ), _userLimit( -1 ) {}

Channel::Channel( std::string name ) : _name( name ), _userLimit( -1 ) {}

Channel::Channel( const Channel& copy ): _name(copy._name) {
	_channelPassword = copy._channelPassword;
	_topic = copy._topic;
	_userLimit = copy._userLimit;
	_inviteOnly = copy._inviteOnly;
	_topicRestriction = copy._topicRestriction;
	_users = copy._users;
	_operators = copy._operators;
	_inviteList = copy._inviteList;
}

Channel::~Channel() {}

Channel&	Channel::operator=( const Channel& assign ) {
	if ( this != &assign )
	{
		_channelPassword = assign._channelPassword;
		_topic = assign._topic;
		_userLimit = assign._userLimit;
		_inviteOnly = assign._inviteOnly;
		_topicRestriction = assign._topicRestriction;
		_users = assign._users;
		_operators = assign._operators;
		_inviteList = assign._inviteList;
	}
	return ( *this );
}

const std::string&	Channel::getPassword() const { return ( _channelPassword ); }

const std::string&	Channel::getName() const { return ( _name ); }

const std::string&	Channel::getTopic() const { return ( _topic ); }

std::map<std::string, User*>&	Channel::getUsers() { return ( _users ); }

std::map<std::string, User*>&	Channel::getOperators() { return ( _operators ); }

std::map<std::string, User*>&	Channel::getInvites() { return ( _inviteList ); }

User*	Channel::getUser( const std::string& nickname ) {
	std::map<std::string, User*>::iterator	it = _operators.find( nickname );
	if ( it != _operators.end() )
		return ( it->second );
	return ( NULL );
}

User*	Channel::getOperator( const std::string& nickname ) {
	std::map<std::string, User*>::iterator	it = _operators.find( nickname );
	if ( it != _operators.end() )
		return ( it->second );
	return ( NULL );
}

int		Channel::getUserLimit() const { return ( _userLimit ); }

int		Channel::getUserCount() const { return ( _users.size() ); }

bool	Channel::getInviteOnly() const { return ( _inviteOnly ); }

bool	Channel::isInvited( std::string& user ) const {
	if ( _inviteList.find( user ) != _inviteList.end() )
		return ( true );
	return ( false );
}

bool	Channel::isUser( std::string& user ) const {
	if ( _users.find( user ) != _users.end() )
		return ( true );
	return ( false );
}

bool	Channel::isOperator( std::string& user ) const {
	if ( _operators.find( user ) != _operators.end() )
		return ( true );
	return ( false );
}

void	Channel::setPassword( std::string password ) { _channelPassword = password; }

void	Channel::setTopic( std::string topic ) { _topic = topic; }

void	Channel::setUserLimit( int userLimit ) { _userLimit = userLimit; }

void	Channel::addUser( User* newUser ) { 
	_users.insert( std::pair<std::string, User*>( ( newUser->getNickname() ), newUser ) ); 
}

void	Channel::addOperator( User* newOperator ) { 
	_operators.insert( std::pair<std::string, User*>( ( newOperator->getNickname() ), newOperator ) );
}

void	Channel::addInvite( User* newInvite ) { 
	_inviteList.insert( std::pair<std::string, User*>( ( newInvite->getNickname() ), newInvite ) ); 
}

void	Channel::ejectUser( User* user ) { _users.erase( user->getUsername() ); }

void	Channel::ejectOperator( User* user ) { _operators.erase( user->getUsername() ); }

void	Channel::removeInvite( User* user ) { _inviteList.erase( user->getUsername() ); }

void	Channel::sendMessage( std::string serverMessage , std::string senderNick ) {
	for ( std::map<std::string, User*>::iterator it = _users.begin(); it != _users.end(); it++ ) {
		if ( it->first == senderNick )
			continue;
		send( it->second->getSocketFd(), serverMessage.c_str(), serverMessage.size(), 0 );
	}
}