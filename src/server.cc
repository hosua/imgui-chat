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
		_sockaddr.sin_port =
			htons((int)strtol(port, nullptr, 10));
		// _sockaddr.sin_addr.s_addr = INADDR_ANY;
		_sockaddr.sin_addr.s_addr = inet_addr(ip);

		if (bind(_sockfd, (sockaddr*)&_sockaddr, sizeof(_sockaddr)) < 0){
			fprintf(stderr, "Failed to bind to port %s\n", port);
		} else {
			printf("Socket created and bound to port %s\n", port);
		}
		
		std::thread thr(&Server::startListening, this);
		thr.detach();
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

			std::thread thr(&Server::handleConnection, this, connection);
			thr.detach();
			// handleConnection(connection);
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
