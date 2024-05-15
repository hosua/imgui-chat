#include "server.hh"

#include <netdb.h>
#include <poll.h>

#include <thread>
#include <mutex>

Server::Server(char* ip, char* port, char* msg_buffer):
	_ip(ip), _port(port), _msg_buffer(msg_buffer){
		_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (_sockfd == -1){
			fprintf(stderr, "Failed to create socket. errno: %i\n", errno);
		}

		_sockaddr.sin_family = AF_INET;
		_sockaddr.sin_port = htons((int)strtol(port, nullptr, 10));
		// _sockaddr.sin_addr.s_addr = INADDR_ANY;
		_sockaddr.sin_addr.s_addr = inet_addr(ip);

		if (bind(_sockfd, (sockaddr*)&_sockaddr, sizeof(_sockaddr)) < 0){
			fprintf(stderr, "Failed to bind to port %s\n", port);
		} else {
			printf("Socket created and bound to port %s\n", port);
		}

		std::thread thr(&Server::startListening, this);
		thr.detach();
		// startListening();
	}

void Server::addMessage(const char* msg){ 
	if (strnlen(msg, 512) > 0)
		_message_log.push_back(std::string(msg));
	else
		fprintf(stderr, "Server received an empty message.\n");
}

void Server::startListening(){
	listen(_sockfd, SOMAXCONN);	
	while (true){
		sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		int connection = accept(_sockfd, (sockaddr*) &client_addr, &client_len);
		if (connection < 0){
			fprintf(stderr, "Failed to establish connection. errno: %i\n", errno);
		} else {
			char client_name[INET_ADDRSTRLEN] = { 0 };
			inet_ntop(AF_INET, &client_addr.sin_addr, client_name, sizeof(client_name) );
			int client_port = htons(client_addr.sin_port);

			printf("Connection Established with client %s:%i\n", client_name, client_port);
			handleConnection(connection);
		}
	}
}

void Server::stopListening(){
	close(_sockfd);
}

void Server::handleConnection(int connection){
	char buffer[1024] = { 0 };
	size_t bytes_read = read(connection, buffer, 512);
	
	char recv_buffer[1024] = { 0 };
	snprintf(recv_buffer, sizeof(recv_buffer), "\n%s\n", buffer);

	strcat(_msg_buffer, recv_buffer);

	close(connection);
}
// Return a listening socket
int Server::getListenerSocket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, _port, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

void Server::addToPfds(std::vector<pollfd> pfds, int new_fd, int& fd_count, int& fd_size){
	if (fd_count == fd_size)
		fd_size *= 2;

	pfds[fd_count].fd = new_fd;
	pfds[fd_count].events = POLLIN; // They see me pollin', they hatin'
	fd_count++;
}

void Server::delFromPfds(std::vector<pollfd> pfds, int i, int& fd_count){
	pfds.erase(pfds.begin()+i);
	fd_count--;
}
