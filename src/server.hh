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
#include <thread>
#include <mutex>

class Server {
public:
	Server(char* ip, char* port, char* msg_buffer);	
	~Server() = default;
	
	void startListening();
	void stopListening();
	void handleConnection(int connection);
	
	void addMessage(const char* msg);
	void setLogToBuffer();

	const std::vector<std::string>& getMessageLog() const { return _message_log; }

private:
	char *_ip, *_port, *_msg_buffer;
	std::vector<std::string> _message_log;
	sockaddr_in _sockaddr;
	int _sockfd;
	std::mutex _connections_mutex;
};
