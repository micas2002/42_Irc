#include "../includes/Channel.hpp"

Channel::Channel() : _name( "" ), _userLimit( -1 ) {}

Channel::Channel( std::string name ) : _name( name ) {}

Channel::Channel( const Channel& copy ) {
	*this = copy;
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

const std::map<std::string, User*>&	Channel::getUsers() const { return ( _users ); }

const std::map<std::string, User*>&	Channel::getOperators() const { return ( _operators ); }

const std::map<std::string, User*>&	Channel::getInvites() const { return ( _inviteList ); }

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

std::string	Channel::getModes() const {
	std::string finalModes = "+";
	std::string addOns = " ";

	if ( _inviteOnly )
		finalModes += 'i';
	else if ( _topicRestriction )
		finalModes += 't';
	else if ( _channelPassword.length() > 0 ) {
		finalModes += 'k';
		addOns += _channelPassword + " ";
	}
	else if ( _userLimit != -1 ) {
		finalModes += 'l';
		addOns += _userLimit;
	}

	return ( finalModes + addOns );
}

bool	Channel::isInvited( User* user ) const {
	if ( _inviteList.find( user->getUsername() ) != _inviteList.end() )
		return ( true );
	return ( false );
}

void	Channel::setPassword( std::string password ) { _channelPassword = password; }

void	Channel::setTopic( std::string topic ) { _topic = topic; }

void	Channel::setUserLimit( int userLimit ) { _userLimit = userLimit; }

void	Channel::setInviteOnly( bool inviteOnly ) { _inviteOnly = inviteOnly; }

void	Channel::setTopicRestriction( bool topicRestriction ) { _topicRestriction = topicRestriction; }

void	Channel::addUser( User* newUser ) { 
	_users.insert( std::pair<std::string, User*>( ( newUser->getNickname() ), newUser ) ); 
}

void	Channel::addOperator( User* newOperator ) { 
	_users.insert( std::pair<std::string, User*>( ( newOperator->getNickname() ), newOperator ) );
}

void	Channel::addInvite( User* newInvite ) { 
	_users.insert( std::pair<std::string, User*>( ( newInvite->getNickname() ), newInvite ) ); 
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

bool	Channel::isOperator( User* user ) {
	std::map<std::string, User*>::iterator	it = _operators.find( user->getNickname() );
	if ( it != _operators.end() )
		return ( true );
	return ( false );
}