#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <poll.h>

class Server {
public:
	Server(char* ip, char* port, char* msg_buffer);	
	~Server() = default;
	
	void startListening();
	void stopListening();
	void handleConnection(int connection);
	
	void addMessage(const char* msg);

	int getListenerSocket(void);
	void addToPfds(std::vector<pollfd> pfds, int newfd, int& fd_count, int& fd_size);
	void delFromPfds(std::vector<pollfd> pfds, int i, int& fd_count);

	const std::vector<std::string>& getMessageLog() const { return _message_log; }

private:
	char *_ip, *_port, *_msg_buffer;
	std::vector<std::string> _message_log;
	sockaddr_in _sockaddr;
	int _sockfd;
};
