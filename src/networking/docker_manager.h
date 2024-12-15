#pragma once

#include <iostream>
#include <string>
#include <list>
#include <utility>
#include "../utils/json.hpp"
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
    std::vector<string> findProxyPorts(const int& amount);
    std::vector<string> findControlPorts(const int& amount);
    std::vector<string> findIPs(const int& amount);
    std::map<string, std::vector<string>> buildCircuits;
    unsigned int amountCreated;
};