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
#include <List>
#include <utility>
#include "WSAInitializer.h"
#include "../utils/Helper.h"

#define COMMUNICATE_NODE_PORT 9050 //inside network
#define CONTROL_NODE_PORT 9051 //inside network
#define COMMUNICATE_Node_PORT 9787


class Node
{
public:
    Node();
    ~Node();
    void serveProxy();
    void serveControl();

private:
    void bindAndListen();
    void acceptClient();
    void clientHandler(const SOCKET client_socket);
    //void runCmdCommand(const std::string command);
    SOCKET _socket;
};