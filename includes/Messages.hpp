#pragma once

// Server messages
#define SERVER_AWAITING_CONNECTIONS "Server: awaiting connections"

#define SERVER_INCORRECT_PASSWORD "Server: Incorrect password! Desconecting from server...\n"
#define SERVER_CORRECT_PASSWORD "Server: Correct password!\n"
#define SERVER_NICKNAME_ADDED "Server: Nickname added!\n"
#define SERVER_NICKNAME_ALREADY_IN_USE "Server: Nickname already in use by another user. Please choose a new one.\n"
#define SERVER_USERNAME_ADDED "Server: Username added!\n"
#define SERVER_INVALID_COMMAND "Server: Invalid command. Please use /PASS, /NICK and /USER commands.\n"

#define SERVER_LOGGED_IN "Server: You're now logged in.\n"


// Colors
#define RESET   "\033[0m"
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
