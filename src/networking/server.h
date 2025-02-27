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
#include <mutex>
#include <set>
#include <condition_variable>
#include <ws2tcpip.h>
#include "WSAInitializer.h"
#include "docker_manager.h"
#include "../utils/Helper.h"
#include "handlers/TorRequestHandler.h"
#include "../utils/RSA.h"
#include "../utils/ECDHE.h"


#define MAX_INT_OF_BYTE 256
#define DEC 1
#define SECONDS_TO_WAIT 5 //the maximum time we wait for node alives

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
        void acceptControlClient();
        void clientHandler(const SOCKET client_socket);

        void clientControlHandler(const SOCKET& node_sock, const std::vector<unsigned int>& circuits, std::string nodeIp);
        void setupSocketTimeout(const SOCKET& node_sock, int seconeds_to_wait = SECONDS_TO_WAIT);
        bool handleCircuitNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock);
        bool checkNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp);
        bool processCircuitNotifications(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock);
        void notifyNodeDeletion(const SOCKET& node_sock, unsigned int circuitId);
        void notifyClientDeletion(unsigned int circuitId);
        void regenerateCircuit(unsigned int circuitId, const std::string& nodeIp);
        bool receiveAliveMessage(const SOCKET& node_sock, const std::string& nodeIp);
        void handleNodeTimeout(const std::vector<unsigned int>& circuits, const std::string& nodeIp, const SOCKET& node_sock);


        std::map<unsigned int, std::vector<std::pair<std::string, std::string>>> _controlList; // nodes data
        std::map<unsigned int, SOCKET> _clients;
        std::map<unsigned int, std::set<std::string>> _circuitsToNotify;
        SOCKET _socket;
        SOCKET _controlSocket;

        ECDHE ecdhe;
                //self, client's
        std::map<SOCKET, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>> _rsaInfo;
        std::map<SOCKET, ECDHE> _ecdheInfo;
        AES _aes;
};