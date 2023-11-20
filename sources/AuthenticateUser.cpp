#include "Server.hpp"

bool	Server::authenticateUser( int newFd ) {
	User		user;
    char 		buffer[1024];
    std::string input;
    int			numBytes;
	bool		passwordMatch = false;

    user.setSocketFd( newFd );
	while ( user.userFieldsEmpty() == true || passwordMatch == false )
	{
		numBytes = recv( newFd, buffer, sizeof( buffer ) - 1, 0 );
		if ( numBytes > 0 ) {
			buffer[numBytes] = '\0';
			char *token = strtok( buffer, "\n" );

			while ( token != NULL ) {
				// Process each line here
				input = token;

				// Check for keywords like "PASS," "NICK," "USER"
				if ( checkAuthenticationCommands( input, newFd, user, passwordMatch ) == false )
					return ( false );

				// Get the next token
				token = strtok( NULL, "\n" );
			}
		}
	}
	addUser( user );
	send( newFd, SERVER_LOGGED_IN,  30, 0 );
	return ( true );
}

bool	Server::checkAuthenticationCommands( std::string& input, int newFd, User& user, bool& passwordMatch ) {
	std::cout << input << std::endl;

	if ( input.find( "PASS" ) != std::string::npos ) {
		input.erase( std::remove(input.begin(), input.end(), '\''), input.end() );

		if ( checkIfPasswordsMatch( input ) == false )
		{
			std::cout << SERVER_INCORRECT_PASSWORD << std::endl;
			send( newFd, SERVER_INCORRECT_PASSWORD,  56, 0 );
			close( newFd );
			FD_CLR( newFd, &_master );
			return ( false );
		}
		passwordMatch = true;
		send( newFd, SERVER_CORRECT_PASSWORD, 26, 0 );
	}
	else if ( input.find( "NICK" ) != std::string::npos ) {
		if ( findDuplicateNicknames( input.substr( strlen( "NICK " ), input.length() - strlen( "NICK " ) ) ) == false ) {
			user.setNickname( input.substr(strlen( "NICK " ), input.length() - strlen( "NICK " ) ) ); ;
			send( newFd, SERVER_NICKNAME_ADDED, 24, 0 );
		}
		else
			send( newFd, SERVER_NICKNAME_ALREADY_IN_USE, 74, 0 );
	}
	else if ( input.find( "USER" ) != std::string::npos ) {
			user.setUsername( input.substr( strlen( "USER " ), input.length() - strlen( "USER " ) ) );	
			send( newFd, SERVER_USERNAME_ADDED, 24, 0 );
	}
	else if ( input.find( "CAP" ) != std::string::npos )
		return ( true );
	else
		send( newFd, SERVER_INVALID_COMMAND, 69, 0 );
	return ( true );
}

bool	Server::checkIfPasswordsMatch(const std::string& input ) const {
	int	passCommandLength = strlen( "PASS " );

	return ( _serverPassword.compare( input.substr( passCommandLength, input.length() - passCommandLength - 1 ) ) == 0 );
}

bool	Server::findDuplicateNicknames( const std::string& nickname ) const {
	if ( _users.find( nickname ) == _users.end() )
		return ( false );
	return ( true );
}