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
#include <ws2tcpip.h>
#include "WSAInitializer.h"
#include "docker_manager.h"
#include "../utils/Helper.h"

class Server
{
    public: 
        Server(); 
        ~Server(); 
        void serve(); 
        void serveControl(std::list<std::pair<std::string, std::string>>& control_info);
        DockerManager dm;

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