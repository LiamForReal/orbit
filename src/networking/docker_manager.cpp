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

std::list<std::string> DockerManager::findIPs(const int& amount)
{
    std::list<std::string> nodesIp;
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
    std::string portsStr = "", hostPort = "";

    for (int i = 0; i < amount; i++)
    {
        // Build the docker inspect command with necessary `cd`
        std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9051/tcp\\\") 0).HostPort }}\" " + std::string(CONTAINER_NAME) + std::to_string(i + 1);

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
            //json portsJson = json::parse(portsStr);
            //hostPort = portsJson["9051/tcp"][0]["HostPort"];
            //std::cout << "container " + std::to_string(i + 1) + " id is " + hostPort + "\n";
            //controlNodesPorts.push_back(hostPort);

            std::cout << "portsStr: " << portsStr << std::endl;
            //json portsJson = json::parse(portsStr);
            //std::cout << "After parse\n";
            hostPort = std::to_string(std::stoi(portsStr));

            std::cout << "container " + std::to_string(i + 1) + " id is " + hostPort + "\n";
            controlNodesPorts.emplace_back(hostPort);
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
    std::string portsStr = "", hostPort = "";
    // Build the docker inspect command with necessary `cd`
    std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9050/tcp\\\") 0).HostPort }}\" " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + 1);
    std::cout << inspectCommand << std::endl;
    // Execute the command
    FILE* pipe = _popen(inspectCommand.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Failed to run inspect command");

    portsStr.clear();
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        printf("BUFFER: %s |ENDED|\n", buffer);
        portsStr += buffer; // Collect JSON output
    }
    _pclose(pipe);

    // Parse the JSON to extract the ports
    try
    {
        std::cout << "portsStr: " << portsStr << std::endl;
        //json portsJson = json::parse(portsStr);
        std::cout << "After parse\n";
        //hostPort = portsJson["9050/tcp"][0]["HostPort"];
        std::cout << "portsStr: " << portsStr << std::endl;
        hostPort = std::to_string(std::stoi(portsStr));

        std::cout << "container " + std::to_string(this->amountCreated + 1) + " id is " + hostPort + "\n";
        proxyNodesPorts.emplace_back(hostPort);
        for (int i = 0; i < amount - 1; i++)
        {
            proxyNodesPorts.emplace_back(INTERNAL_PORT);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "ERROR HERE\n";
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
        nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
        itPort++;
        itIp++;
    }
    this->amountCreated += create;
    return nodesInfo;
}

std::list<std::pair<std::string, std::string>> DockerManager::GetControlInfo() //for server
{
    std::list<std::pair<std::string, std::string>> nodesInfo;
    std::list<std::string> ips = findIPs(this->amountCreated);
    std::list<std::string> ports = findControlPorts(this->amountCreated);
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