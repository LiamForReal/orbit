#include "docker_manager.h"
#include <random>

DockerManager::DockerManager() 
{ 
    this->amountCreated = 0; 
    runCmdCommand("docker network rm dockerfiles_TOR_NETWORK");
    runCmdCommand("python ../dockerFiles/docker_node_info_init.py"); //pip install pyyaml - to run it
}

void DockerManager::runCmdCommand(const std::string& command)
{
    int result;
    result = system(command.c_str());
    if (result == 0) 
        std::cout << "Command process started successfully." << std::endl;
    else std::cerr << "Failed to start command's process. Error code: " << result << std::endl;
}

void DockerManager::openDocker(const int& amount)
{
    string portsCommand = "python ../dockerFiles/adjust_ports_to_talk_in_subnet.py";
    string firstNodeName = "";
    std::vector<string> nodesNames;
    std::string buildCommand = "cd ../dockerFiles/ && docker-compose -f Docker-compose.yaml up --build -d";
    if (this->amountCreated + amount >= 20)
        throw std::runtime_error("to many nodes the server cant allow it!");
    for (int i = 0; i < amount; i++)
    {
        buildCommand += " " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1);

        if (i != 0)
        {
            portsCommand += " " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1);
            //nodesNames.emplace_back(std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1));
        }

        //if (i != this->amountCreated)
        //{
        //    portsCommand += " " + std::string(CONTAINER_NAME) + std::to_string(i + 1);
        //    nodesNames.emplace_back(std::string(CONTAINER_NAME) + std::to_string(i + 1));
        //}
        //else firstNodeName = std::string(CONTAINER_NAME) + std::to_string(i + 1);
    }

    //buildCircuits[firstNodeName] = nodesNames; //for next 

    runCmdCommand(portsCommand);

    runCmdCommand(buildCommand);
}

std::vector<std::string> DockerManager::findIPs(const int& amount)
{
    std::vector<std::string> nodesIp;
    char buffer[128];
    std::string containerID;
    //int random_number = min + std::rand() % (max - min + 1); TODO: access the nodes for all the clients!!!
    for (int i = 0; i < amount; i++)
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
        std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{.NetworkSettings.Networks.dockerfiles_TOR_NETWORK.IPAMConfig.IPv4Address }}\" " + std::string(CONTAINER_NAME) + char(i + 49);
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

std::vector<std::string> DockerManager::findControlPorts(const int& amount)
{
    std::vector<std::string> controlNodesPorts;
    char buffer[128];
    std::string portsStr = "", hostPort = "";
    std::string inspectCommand;

    for (int i = 0; i < amount; i++)
    {
        // Build the docker inspect command with necessary `cd`
        inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9051/tcp\\\") 0).HostPort }}\" " + std::string(CONTAINER_NAME) + std::to_string(i + 1);

        // Execute the command
        FILE* pipe = _popen(inspectCommand.c_str(), "r");
        if (!pipe)
            throw std::runtime_error("Failed to run inspect command");

        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            portsStr += buffer; // Collect JSON output
        }
        _pclose(pipe);
        try
        {
            hostPort = std::to_string(std::stoi(portsStr));
            std::cout << "container " << std::to_string(i + 1) << "control port id is "  << hostPort << "\n";
            controlNodesPorts.emplace_back(hostPort);
            portsStr = "";
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error parsing JSON: " << ex.what() << std::endl;
        }
    }
    return controlNodesPorts;
}

std::vector<std::string> DockerManager::findProxyPorts(const int& amount)
{
    std::vector<std::string> proxyNodesPorts;
    char buffer[128];
    std::string portsStr = "", hostPort = "";
    // Build the docker inspect command with necessary `cd`
    std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9050/tcp\\\") 0).HostPort }}\" " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + 1);
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
        hostPort = std::to_string(std::stoi(portsStr));

        std::cout << "container " << std::to_string(this->amountCreated + 1) << " proxy port is " << hostPort << "\n";
        proxyNodesPorts.emplace_back(hostPort);
        for (int i = 1; i < amount; i++)
        {
            std::cout << "container " << std::to_string(this->amountCreated + i + 1) << " proxy port is 9050\n";
            proxyNodesPorts.emplace_back(INTERNAL_PORT);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing JSON: " << ex.what() << std::endl;
    }

    
    return proxyNodesPorts;
}

std::vector<std::pair<std::string, std::string>> DockerManager::openAndGetInfo(const int& use, const int& create)
{
    openDocker(create);
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<std::string> ips = findIPs(use); 
    std::vector<std::string> ports = findProxyPorts(use);
    auto itIp = ips.begin();
    auto itPort = ports.begin();
    for (int i = 0; i < ips.size(); i++)
    {
        nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
        itPort++;
        itIp++;
    }
    this->amountCreated += create;
    return nodesInfo;
}

std::vector<std::pair<std::string, std::string>> DockerManager::GetControlInfo() //for server
{
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<std::string> ips = findIPs(this->amountCreated);
    std::vector<std::string> ports = findControlPorts(this->amountCreated);
    auto itIp = ips.begin();
    auto itPort = ports.begin();
    for (int i = 0; i < ips.size(); i++)
    {
        nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
        itPort++;
        itIp++;
    }
    return nodesInfo;
}