#pragma comment (lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include "WSAInitializer.h"
#define COMMUNICATE_NODE_PORT 9050
#define COMMUNICATE_SERVER_PORT 9787
#define MAX_NODES_TO_OPEN 5
#define MIN_NODES_TO_OPEN 1
class Client
{
public:
	Client();
	~Client();
	void connectToServer(std::string serverIP, int port);
	void startConversation();

private:
	SOCKET _clientSocket;
};