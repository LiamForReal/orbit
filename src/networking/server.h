#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h> 
#include <iostream>
#include <cstdlib> //for system
#include<stdlib.h>
#include <exception> 
#include <string>
#include <thread>  
#include <vector>
#include <List>
#include "WSAInitializer.h"
#include "docker_manager.h"
#include "../utils/Requestes.hpp"
#include "../utils/Responses.hpp"
#include "../utils/DeserializerRequests.h"
#include "../utils/SerializerResponses.h"
#include "../utils/Helper.h"

#define COMMUNICATE_CONTROL_PORT 9051
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
        void runCmdCommand(const std::string command);
        SOCKET _socket;
};