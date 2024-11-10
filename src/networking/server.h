#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h> 
#include <iostream>
#include <exception> 
#include <string>
#include <thread>  
#include <vector>
#include <List>
#include "WSAInitializer.h"

class Server
{
    public: 
        Server(); 
        ~Server(); 
        void serve(); 

    private: 
        void bindAndListen(); 
        void acceptClient(); 
        void clientHandler(const SOCKET client_socket);

        SOCKET _socket;
};