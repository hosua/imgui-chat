#include "client.hh"
#include <cstdio>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <thread>

int Client::connectToServer(const char* ip, const char* port){
	sockaddr_in serv_addr = { 0 };
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons((int)strtol(port, nullptr, 10));
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0){
		fprintf(stderr, "Error: pton error\n");
		return -1;
	}
	
	if ( (_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error: Failed to create socket\n");
		return -1;
	}

	if ( connect(_sockfd, (struct sockaddr *)& serv_addr, sizeof(serv_addr)) < 0){
		fprintf(stderr, "Error: Connection failed.\n");
		return -1;
	}
	return 0;
}

void Client::disconnectFromServer(){
	
}

Client::Client(const char* ip, const char* port, char* client_buffer):
	_client_buffer(client_buffer) {
	// serv_addr.sin_port = htons(8989);
	
	connectToServer(ip, port);

	std::thread thr(&Client::readServer, this);
	thr.detach();
	// readServer(sockfd);
}

void Client::readServer(){
	char recv_buffer[1024] = { 0 };
	int n = 0;
	while ( (n = read(_sockfd, recv_buffer, sizeof(recv_buffer)-1)) > 0){
		recv_buffer[n] = 0;
		fprintf(stdout, "%s\n", recv_buffer);
	}

	if (n < 0)
		printf("\nRead error\n");
}

void Client::sendMessage(){
	send(_sockfd, _client_buffer, strlen(_client_buffer), 0);
	close(_sockfd);
}
