#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 3) {
		std::cerr << "usage: client <hostname> <port>\n";
		return 1;
	}

	// Set up the hints structure
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	// Get address info
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
		return 1;
	}

	// Loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			std::perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			std::perror("client: connect");
			continue;
		}

		break; // If we get here, we must have connected successfully
	}

	if (p == NULL) {
		std::cerr << "client: failed to connect\n";
		return 2;
	}


	// inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	//           s, sizeof s);
	// std::cout << "client: connecting to " << s << "\n";

	freeaddrinfo(servinfo); // All done with this structure

	// Communicate with the server...
	// Send a message to the server
	std::string msg;
	while (1)
	{
		std::getline(std::cin, msg);
	
		if (send(sockfd, msg.c_str(), msg.length(), 0) == -1) {
			std::perror("send");
			close(sockfd);
			exit(1);
		}
	}

	close(sockfd);

	return 0;
}
