#pragma once

#include <iostream>
#include <string>
#include <list>
#include <utility>
#include "../utils/json.hpp"
#include <algorithm>

#define CONTAINER_NAME "node"
#define INTERNAL_PORT "9050"

using json = nlohmann::json;
using std::string;
using std::pair;

class DockerManager
{
public:
    DockerManager();

    std::vector<pair<string, string>> openAndGetInfo(const int& use, const int& create);
    std::vector<pair<string, string>> GetControlInfo();
    std::vector<std::pair<std::string, std::string>> adjustCrushedNodes(std::vector<string> crushedNodes, const int& use);

private:
    void runCmdCommand(const string& command);
    void openDocker(const int& amount);
    void openDockerByContanerName(std::vector<string>& contanersNames);
    void setNewNodes(const int& create, const int& use);
    std::vector<string> SelectPathAndAdjustNetwork(int use);
    std::vector<string> findProxyPorts(std::vector<string> containersNames);
    std::vector<string> findControlPorts(std::vector<string> containersNames);
    std::vector<string> findIPs(std::vector<string> containersNames);
    
    std::vector<string> pathNodeExisting; // after sorting round robin first node so for 3 nodes it will be all the nodes ecxept the guerd node,
    std::vector<string> guardNodeExisting; //after sorting round robin first node so for 3 nodes it will be node1||node2||node3,
                                            //this vector size equales to the number of clients
    std::vector<string> unDefinedNodes; //every time a client want to open node it gets here for eaxmple if he wanna open 3 it will be node1 noed2 noed3
    unsigned int amountCreated;
    unsigned int _clientsAmount;
};