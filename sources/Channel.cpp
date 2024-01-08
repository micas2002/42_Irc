#include "../includes/Channel.hpp"

Channel::Channel() : _name( "" ), _userLimit( -1 ), _inviteOnly( false ), _topicRestriction( false ) {}

Channel::Channel( std::string name ) : _name( name ), _userLimit( -1 ), _inviteOnly( false ), _topicRestriction( false ) {}

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

std::set<std::string>&	Channel::getOperators() { return ( _operators ); }

std::set<std::string>&	Channel::getInvites() { return ( _inviteList ); }

User*	Channel::getUser( const std::string& nickname ) {
	std::map<std::string, User*>::iterator	it = _users.find( nickname );
	if ( it != _users.end() )
		return ( it->second );
	return ( NULL );
}

int		Channel::getUserLimit() const { return ( _userLimit ); }

int		Channel::getUserCount() const { return ( _users.size() ); }

bool	Channel::getInviteOnly() const { return ( _inviteOnly ); }

bool	Channel::getTopicRestriction() const { return ( _topicRestriction ); }

bool	Channel::isInvited( const std::string& user ) const {
	if ( _inviteList.find( user ) != _inviteList.end() )
		return ( true );
	return ( false );
}

bool	Channel::isUser( const std::string& user ) const {
	if ( _users.find( user ) != _users.end() )
		return ( true );
	return ( false );
}

bool	Channel::isOperator( const std::string& user ) const {
	if ( _operators.find( user ) != _operators.end() )
		return ( true );
	return ( false );
}

std::string	Channel::getModes() const {
	std::string			finalModes = "+";
	std::string			addOns = "";
	std::stringstream	ss;

	if ( _inviteOnly )
		finalModes += 'i';
	if ( _topicRestriction )
		finalModes += 't';
	if ( _userLimit != -1 ) {
		finalModes += 'l';
		ss << _userLimit;
		ss >> addOns;
	}
	if ( _channelPassword.length() > 0 ) {
		finalModes += 'k';
		if ( addOns.length() > 0 )
			addOns += " ";
		addOns += _channelPassword;
	}
	if ( finalModes.length() == 1 )
		return ( "" );
	return ( finalModes + " " + addOns );
}

void	Channel::setPassword( std::string password ) { _channelPassword = password; }

void	Channel::setTopic( std::string topic ) { _topic = topic; }

void	Channel::setUserLimit( int userLimit ) { _userLimit = userLimit; }

void	Channel::setInviteOnly( bool inviteOnly ) { _inviteOnly = inviteOnly; }

void	Channel::setTopicRestriction( bool topicRestriction ) { _topicRestriction = topicRestriction; }

void	Channel::addUser( User* newUser ) { 
	_users.insert( std::pair<std::string, User*>( ( newUser->getNickname() ), newUser ) ); 
}

void	Channel::addOperator( const std::string& newOperator ) { 
	_operators.insert( newOperator );
}

void	Channel::addInvite( const std::string& newInvite ) { 
	_inviteList.insert( newInvite ); 
}

void	Channel::ejectUser( const std::string& user ) { _users.erase( user ); }

void	Channel::ejectOperator( const std::string& user ) { _operators.erase( user ); }

void	Channel::removeInvite( const std::string& invite ) { _inviteList.erase( invite ); }

void	Channel::sendMessage( std::string serverMessage , std::string senderNick ) {
	for ( std::map<std::string, User*>::iterator it = _users.begin(); it != _users.end(); it++ ) {
		if ( it->first == senderNick )
			continue;
		send( it->second->getSocketFd(), serverMessage.c_str(), serverMessage.size(), 0 );
	}
}
