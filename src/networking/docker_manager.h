#pragma once

#include <iostream>
#include <string>
#include <list>
#include <utility>
#include "../utils/json.hpp"
#include <algorithm>
#include <mutex>
#define CONTAINER_NAME "node"
#define INTERNAL_PORT "9050"
#define NODE_CAPACITY 10 
using json = nlohmann::json;
using std::string;
using std::pair;

class DockerManager
{
public:
    DockerManager();
    std::vector<pair<string, string>> openAndGetInfo(const int& use, const int& create, const unsigned int circuitId);
    std::vector<pair<string, string>> GetControlInfo();
    std::vector<std::pair<std::string, std::string>> giveCircuitAfterCrush(string crushedNode, const int use, const unsigned int circuitId);

private:
    void deleteDockerContainers();
    void runCmdCommand(const string& command);
    std::vector<std::string> runCommand(const std::string& command);
    void openDocker(const int& amount);
    void openDocker(const string& containerName);
    void setNewNodes(const int& create, const int& use);
    std::vector<string> SelectPathAndAdjustNetwork(int use, const unsigned int circuitId);
    std::vector<string> findProxyPorts(std::vector<string>& containersNames);
    std::vector<string> findControlPorts(std::vector<string>& containersNames);
    std::vector<string> findIPs(std::vector<string>& containersNames);

    std::vector<string> pathNodeExisting; // after sorting round robin first node so for 3 nodes it will be all the nodes ecxept the guerd node,
    std::map<unsigned int, string> guardNodeCircuits; //after sorting round robin first node so for 3 nodes it will be node1||node2||node3,
    //this map connects gured node to client
    std::vector<string> unDefinedNodes; //every time a client want to open node it gets here for eaxmple if he wanna open 3 it will be node1 noed2 noed3
    unsigned int amountCreated;
    unsigned int _clientsAmount;
};