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
#include <list>
#include <map>
#include <utility>
#include <ws2tcpip.h>
#include "WSAInitializer.h"
#include "docker_manager.h"
#include "../utils/Helper.h"
#include <mutex>
#include<set>
#include<condition_variable>
#include "handlers/TorRequestHandler.h"

#define MAX_INT_OF_BYTE 256
#define DEC 1
#define SECONDS_TO_WAIT 10 //the maximum time we wait for node alives
#define AMOUNT_OF_BYTES 1250

class Server
{
    public: 
        Server(); 
        ~Server(); 
        void serve();
        void serveClients();
        void serveControl();

    private: 
        SOCKET createSocket(const std::string& ip, unsigned int port);
        void bindAndListen(); 
        void bindAndListenControl();
        void acceptClient(); 
        void acceptControlClient(const std::vector<string>& allowedClients);
        void clientHandler(const SOCKET client_socket);
        void clientControlHandler(const SOCKET node_sock, const std::vector<unsigned int>& circuits, string nodeIp);
        
        std::map<unsigned int, std::vector<std::pair<std::string, std::string>>> _controlList; // nodes data
        std::map<unsigned int, SOCKET> _clients;
        std::map<unsigned int, std::set<std::string>> _circuitsToNotify;
        SOCKET _socket;
        SOCKET _controlSocket;
};