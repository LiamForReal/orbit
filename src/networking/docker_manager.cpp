#include "docker_manager.h"
#include <random>
DockerManager::DockerManager() { this->amountCreated = 0; }

void DockerManager::runCmdCommand(const std::string& command)
{
    int result;
    result = system(command.c_str());
    if (result == 0) 
        std::cout << "Docker Compose process started successfully." << std::endl;
    else std::cerr << "Failed to start Docker Compose process. Error code: " << result << std::endl;
}

void DockerManager::openDocker(const int& amount)
{
    const std::string containerName = "node";
    std::string command = "cd ../dockerFiles/ && docker-compose up --build -d";  // -d flag runs it in detached mode
    if (this->amountCreated + amount >= 20)
        throw std::runtime_error("to many nodes the server cant allow it!");
    for (int i = this->amountCreated ; i < amount; i++)
    {
        command += " " + std::string(CONTAINER_NAME) + std::to_string(i + 1); 
    }
    runCmdCommand("python ../dockerFiles/docker_node_info_init.py"); //pip install pyyaml - to run it
    runCmdCommand(command);
}

std::list<std::string> DockerManager::findIPs(const int& amount)
{
    std::list<std::string> nodesIp;
    char buffer[128];
    std::string containerID;
    //int random_number = min + std::rand() % (max - min + 1); TODO: access the nodes for all the clients!!!
    for (int i = this->amountCreated; i < this->amountCreated + amount; i++)
    {
        std::string containerIDCommand = "cd ../dockerFiles/ && docker-compose ps -q";
        FILE* pipe = _popen(containerIDCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run command");
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerID += buffer;
        }
        _pclose(pipe);
        containerID = containerID.substr(0, containerID.find("\n"));  // Clean up newlines
        std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}\" " + std::string(CONTAINER_NAME) + char(i + 49);
        std::cout << inspectCommand << std::endl; 
        pipe = _popen(inspectCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run inspect command");
        std::string containerIP;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerIP += buffer;
        }
        _pclose(pipe);
	    containerIP = containerIP.substr(0, containerIP.find("\n"));
	    nodesIp.push_back(containerIP);
    }

    return nodesIp;
}

std::list<std::string> DockerManager::findControlPorts(const int& amount)
{
    std::list<std::string> controlNodesPorts;
    char buffer[128];
    int j;
    std::string portsStr, hostPort = "";

    for (int i = this->amountCreated; i < this->amountCreated + amount; i++)
    {
        // Build the docker inspect command with necessary `cd`
        std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{json .NetworkSettings.Ports}}\" " + std::string(CONTAINER_NAME) + std::to_string(i + 1);

        // Execute the command
        FILE* pipe = _popen(inspectCommand.c_str(), "r");
        if (!pipe)
            throw std::runtime_error("Failed to run inspect command");

        portsStr.clear();
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            portsStr += buffer; // Collect JSON output
        }
        _pclose(pipe);

        // Parse the JSON to extract the ports
        try
        {
            json portsJson = json::parse(portsStr);
            hostPort = portsJson["9051/tcp"][0]["HostPort"];
            std::cout << "container " + std::to_string(i + 1) + " id is " + hostPort + "\n";
            controlNodesPorts.push_back(hostPort);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error parsing JSON: " << ex.what() << std::endl;
        }
    }
    return controlNodesPorts;
}

std::list<std::string> DockerManager::findProxyPorts(const int& amount)
{
    std::list<std::string> proxyNodesPorts;
    char buffer[128];
    int j;
    std::string portsStr, hostPort = "";

    // Build the docker inspect command with necessary `cd`
    std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{json .NetworkSettings.Ports}}\" " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + 1);

    // Execute the command
    FILE* pipe = _popen(inspectCommand.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Failed to run inspect command");

    portsStr.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        portsStr += buffer; // Collect JSON output
    }
    _pclose(pipe);

    // Parse the JSON to extract the ports
    try
    {
        json portsJson = json::parse(portsStr);
        hostPort = portsJson["9050/tcp"][0]["HostPort"];
        std::cout << "container " + std::to_string(this->amountCreated + 1) + " id is " + hostPort + "\n";
        proxyNodesPorts.push_back(hostPort);
        for (int i = this->amountCreated + 1; i < this->amountCreated + amount; i++)
        {
            proxyNodesPorts.emplace_back(INTERNAL_PORT);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing JSON: " << ex.what() << std::endl;
    }

    return proxyNodesPorts;
}

std::list<std::pair<std::string, std::string>> DockerManager::openAndGetInfo(const int& use, const int& create)
{
    openDocker(create);
    std::list<std::pair<std::string, std::string>> nodesInfo;
    std::list<std::string> ips = findIPs(use); 
    std::list<std::string> ports = findProxyPorts(use);
    auto itIp = ips.begin();
    auto itPort = ports.begin();
    for (int i = 0; i < ips.size(); i++)
    {
        std::advance(itPort, i);
        std::advance(itIp, i);
        nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
    }
    this->amountCreated = this->amountCreated + create;
    return nodesInfo;
}

std::list<std::pair<std::string, std::string>> DockerManager::GetControlInfo(const int amount) //for server
{
    std::list<std::pair<std::string, std::string>> nodesInfo;
    std::list<std::string> ips = findIPs(amount);
    std::list<std::string> ports = findControlPorts(amount);
    auto itIp = ips.begin();
    auto itPort = ports.begin();
    for (int i = 0; i < ips.size(); i++)
    {
        std::advance(itPort, i);
        std::advance(itIp, i);
        nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
    }
    return nodesInfo;
}