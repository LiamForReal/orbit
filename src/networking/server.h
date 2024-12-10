#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h> 
#include <iostream>
#include <cstdlib> //for system
#include <stdlib.h>
#include <exception> 
#include <string>
#include <thread>  
#include <vector>
#include <List>
#include <utility>
#include "WSAInitializer.h"
#include "docker_manager.h"
#include "../utils/Helper.h"
#include <ws2ipdef.h>

class Server
{
    public: 
        Server(); 
        ~Server(); 
        void serve(); 
        void serveControl(std::list<std::pair<std::string, std::string>>& control_info);
        
    private: 
        void bindAndListen(); 
        void bindAndListenControl();
        void acceptClient(); 
        void acceptControlClient(const std::list<std::pair<std::string, std::string>>& allowedClients);
        void clientHandler(const SOCKET client_socket);
        void clientControlHandler(const SOCKET node_sock);
        
        SOCKET _socket;
        SOCKET _controlSocket;
};