#pragma once

#include <iostream>
#include <string>
#include <list>
#include <utility>
#include "../utils/json.hpp"
#define CONTAINER_NAME "node"
using json = nlohmann::json;

class DockerManager
{
public:
    DockerManager();

    std::list<std::pair<std::string, std::string>> openAndGetInfo(const int& use, const int& create);
    std::list<std::pair<std::string, std::string>> GetControlInfo(const int amount);

private:
    void runCmdCommand(const std::string& command);
    void openDocker(const int& amount);
    std::list<std::string> findProxyPorts(const int& amount);
    std::list<std::string> findControlPorts(const int& amount);
    std::list<std::string> findIPs(const int& amount);
    unsigned int amountCreated;
};