#pragma once

class Client {
public:
	Client(const char* ip, const char* port, char* client_buffer);	
	~Client() = default;
	
	int connectToServer(const char* ip, const char* port);
	void disconnectFromServer();
	
	void readServer();
	void sendMessage();
private:
	char *_client_buffer;
	int _sockfd;
};
