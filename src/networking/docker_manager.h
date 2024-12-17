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

private:
    void runCmdCommand(const string& command);
    void openDocker(const int& amount);
    void setNewNodes(const int& create, const int& use);
    std::vector<string> SelectPathAndAdjustNetwork(int use);
    std::vector<string> findProxyPorts(std::vector<string> containersNames);
    std::vector<string> findControlPorts(std::vector<string> containersNames);
    std::vector<string> findIPs(std::vector<string> containersNames);
    void adjustCrushedNodes(std::vector<string> crushedNodes);

    std::vector<string> pathNodeExisting;
    std::vector<string> guardNodeExisting;
    std::vector<string> unDefinedNodes;
    unsigned int amountCreated;
    unsigned int _clientsAmount;
};