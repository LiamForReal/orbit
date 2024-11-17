#pragma once

#include <iostream>
#include <string>
#include <list>

#define CONTAINER_NAME "node"

class DockerManager
{
    public:
        DockerManager();

        static std::list<std::string> openAndGetIPs(const int& use, const int& create);

    private:
        static void runCmdCommand(const std::string& command);
        static void openDocker(const int& amount);
        static std::list<std::string> findIPs(const int& amount);
};