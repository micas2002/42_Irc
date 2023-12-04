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

int		Channel::getUserLimit() const { return ( _userLimit ); }

int		Channel::getUserCount() const { return ( _users.size() ); }

bool	Channel::getInviteOnly() const { return ( _inviteOnly ); }

bool	Channel::isInvited( User* user ) const {
	if ( _inviteList.find( user->getUsername() ) != _inviteList.end() )
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