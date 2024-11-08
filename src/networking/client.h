#pragma comment (lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include "WSAInitializer.h"
#include "../utils/SerializerRequests.h"

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