#include "../includes/Channel.hpp"

Channel::Channel() : _name( "" ) {}

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

void	Channel::setPassword( std::string password ) { _channelPassword = password; }

void	Channel::setTopic( std::string topic ) { _topic = topic; }

void	Channel::setUserLimit( int userLimit ) { _userLimit = userLimit; }

void	Channel::addUser( User* newUser ) { 
	_users.insert( std::pair<std::string, User*>( ( newUser->getUsername() ), newUser ) ); 
}

void	Channel::addOperator( User* newOperator ) { 
	_users.insert( std::pair<std::string, User*>( ( newOperator->getUsername() ), newOperator ) );
}

void	Channel::addInvite( User* newInvite ) { 
	_users.insert( std::pair<std::string, User*>( ( newInvite->getUsername() ), newInvite ) ); 
}

void	Channel::ejectUser( User* user ) { _users.erase( user->getUsername() ); }
void	Channel::ejectOperator( User* user ) { _operators.erase( user->getUsername() ); }
void	Channel::removeInvite( User* user ) { _inviteList.erase( user->getUsername() ); }