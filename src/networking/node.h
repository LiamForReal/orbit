#pragma comment (lib, "ws2_32.lib")
#include <chrono> 
#include <WinSock2.h>
#include <Windows.h> 
#include <iostream>
#include <cstdlib> //for system
#include <stdlib.h>
#include <exception> 
#include <string>
#include <thread>  
#include <vector>
#include <map>
#include <cstdlib> // for getenv
#include <mutex>
#include "WSAInitializer.h"
#include "../utils/Helper.h"
#include "../utils/SerializerResponses.h"
#include "../utils/DeserializerRequests.h"
#include "../utils/SerializerRequests.h"
#include "../utils/DeserializerResponses.h"
#include "handlers/NodeRequestHandler.h"
#include "../utils/RSA.h"

#define COMMUNICATE_NODE_PORT 9050 //inside network
#define CONTROL_NODE_PORT 9051 //inside network
#define COMMUNICATE_CONTROL_NODE_PORT 9788 


class Node
{
public:
    Node();
    ~Node();
    void serveProxy(const std::string& ip, uint16_t port);
    void serveControl();
    static std::string getEnvVar(const LPCSTR& key);

private:
    SOCKET createSocketWithServer();
    void bindAndListen(const std::string& ip, uint16_t port);
    void acceptClient();
    void clientHandler(SOCKET client_socket);
    void controlReceiver(SOCKET& serverSock);
    void controlSender(SOCKET& serverSock);

    //NodeRequestHandler nodeRequestHandler;
    std::map<unsigned int, std::pair<SOCKET, SOCKET>> circuits;
    SOCKET _socket;
    //                                  clients RSA public_key and product
    std::map<unsigned int, std::pair<RSA, std::pair<uint2048_t, uint2048_t>>> _rsaKeys;
    std::map<unsigned int, AES> _aesKeys;
};