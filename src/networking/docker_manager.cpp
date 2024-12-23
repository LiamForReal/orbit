#include "docker_manager.h"

#include <random>

DockerManager::DockerManager() 
{ 
    _clientsAmount = 0;
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

void DockerManager::setNewNodes(const int& create, const int& use)
{
    if (this->amountCreated + create >= 20)
        throw std::runtime_error("to many nodes the server cant allow it!");

    for (int i = 0; i < create; i++) //put all the nodes I wanna work with node1 - noden
        unDefinedNodes.emplace_back(std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1));
    //node2 node3 node4 , node1 node8 node6
    if (use > pathNodeExisting.size() + unDefinedNodes.size())//chack use confirmation
        throw std::runtime_error("use is grater then all the nodes exisiting");

}

std::vector<string> DockerManager::SelectPathAndAdjustNetwork(int use)
{
    std::vector<string> nodeSelected;
    std::vector<string> tmp;
    for (int i = 0; i < unDefinedNodes.size(); ++i)  //node selected get all the nodes and choose unique paths to every client each
    {
        nodeSelected.emplace_back(unDefinedNodes[(i + _clientsAmount) % unDefinedNodes.size()]);
    }
    for (auto it = nodeSelected.begin(); it != nodeSelected.end(); ++it)
    {
        if (*it != *nodeSelected.begin())
        {
            pathNodeExisting.emplace_back(*it);  
        }
        else guardNodeExisting.emplace_back(*it); 
    }
    nodeSelected.clear();
    for (int i = 1; i < use; ++i)  //node selected get all the nodes and choose unique paths to every client each
    {
        tmp.emplace_back(pathNodeExisting[i % pathNodeExisting.size()]);
    }
    nodeSelected.emplace_back(guardNodeExisting[_clientsAmount - 1]);
    nodeSelected.insert(nodeSelected.end(), tmp.begin(), tmp.end());
    string portsCommand = "python ../dockerFiles/adjust_ports_to_talk_in_subnet.py";
    std::cout << "CLIENT" << _clientsAmount << " PATH:\n [ " << *nodeSelected.begin() << ", ";
    for (auto it = nodeSelected.begin() + 1; it != nodeSelected.end(); it++)
    {
        portsCommand += " " + *it;
        std::cout << *it << ", ";
    }
    std::cout << "]\n";
    runCmdCommand(portsCommand);
    unDefinedNodes.clear();
    return nodeSelected;
}
void DockerManager::openDocker(const int& amount)
{
    string firstNodeName = "";
    std::vector<string> nodesNames;
    std::string buildCommand = "cd ../dockerFiles/ && docker-compose -f Docker-compose.yaml up --build -d";
    for (int i = 0; i < amount; i++)
    {
        buildCommand += " " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1);
    }

    runCmdCommand(buildCommand);
}

void DockerManager::openDockerByContanerName(std::vector<string>& contanersNames)
{
    string firstNodeName = "";
    std::vector<string> nodesNames;

    std::string regenerateIpsCommend = "python ../dockerFiles/docker_node_info_init.py";
    std::string buildCommand = "cd ../dockerFiles/ && docker-compose -f Docker-compose.yaml up --build -d";
    for (auto it = contanersNames.begin(); it != contanersNames.end(); ++it)
    {
        regenerateIpsCommend += " " + *it;
        buildCommand += " " + *it;
    }

    runCmdCommand(regenerateIpsCommend);
    runCmdCommand(buildCommand);
}


std::vector<std::string> DockerManager::findIPs(std::vector<string> containersNames)
{
    std::vector<std::string> nodesIp;
    char buffer[128];
    std::string containerID;
    //int random_number = min + std::rand() % (max - min + 1); TODO: access the nodes for all the clients!!!
    std::string containerIDCommand = "cd ../dockerFiles/ && docker-compose ps -q";
    for (auto it = containersNames.begin(); it != containersNames.end(); ++it)
    {
        
        FILE* pipe = _popen(containerIDCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run command");
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerID += buffer;
        }
        _pclose(pipe);
        containerID = containerID.substr(0, containerID.find("\n"));  // Clean up newlines
        std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{.NetworkSettings.Networks.dockerfiles_TOR_NETWORK.IPAMConfig.IPv4Address }}\" " + *it;
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

std::vector<std::string> DockerManager::findControlPorts(std::vector<string> containersNames)
{
    std::vector<std::string> controlNodesPorts;
    char buffer[128];
    std::string portsStr = "", hostPort = "";
    std::string inspectCommand;

    for (auto it = containersNames.begin(); it != containersNames.end(); ++it)
    {
        // Build the docker inspect command with necessary `cd`
        inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9051/tcp\\\") 0).HostPort }}\" " + *it;

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
            std::cout <<  *it << ": control port id is "  << hostPort << "\n";
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

std::vector<std::string> DockerManager::findProxyPorts(std::vector<string> containersNames)
{
    std::vector<std::string> proxyNodesPorts;
    char buffer[128];
    std::string portsStr = "", hostPort = "";
    // Build the docker inspect command with necessary `cd`
    std::string inspectCommand = "cd ../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9050/tcp\\\") 0).HostPort }}\" " + *containersNames.begin();
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
        for (int i = 1; i < containersNames.size(); i++)
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
    try
    {
        _clientsAmount++;
        std::vector<std::pair<std::string, std::string>> nodesInfo;
        setNewNodes(create, use);
        std::vector<string> nodeSelected = SelectPathAndAdjustNetwork(use);
        openDocker(create);
        std::vector<std::string> ips = findIPs(nodeSelected);
        std::vector<std::string> ports = findProxyPorts(nodeSelected);
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
    catch (std::runtime_error& e)
    {
        std::cerr << "amount to use is grater then the amount of exisiting nodes!!!\n";
        return std::vector<std::pair<std::string, std::string>>();
    }
}

std::vector<std::pair<std::string, std::string>> DockerManager::GetControlInfo() //for server
{
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<string> nodeExisting;
    nodeExisting.insert(nodeExisting.end(), guardNodeExisting.begin(), guardNodeExisting.end());
    nodeExisting.insert(nodeExisting.end(), pathNodeExisting.begin(), pathNodeExisting.end());
    std::vector<std::string> ips = findIPs(nodeExisting);
    std::vector<std::string> ports = findControlPorts(nodeExisting);
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

std::vector<std::pair<std::string, std::string>> DockerManager::giveCircuitAfterCrush(string crushedNode, const int& use) // list of ips
{
    try
    {
        std::vector<std::pair<std::string, std::string>> nodesInfo;
        std::vector<string> nodes;
        string nodeName;
        int num = 1;
        for (int i = 1; i <= NODE_CAPACITY; i++)
            nodes.emplace_back(CONTAINER_NAME + std::to_string(i));
        std::vector<std::string> ips = findIPs(nodes);
        //std::vector<std::string> ports = findProxyPorts(nodeSelected);
        for (auto it = nodes.begin(); it != nodes.end(); ++it)
        {
            if (crushedNode == *it)
            {
                nodeName = CONTAINER_NAME + std::to_string(num);
            }
            num++;
        } //TO FIX
        std::vector<string> nodeSelected = SelectPathAndAdjustNetwork(use);

        auto itIp = ips.begin();
        //auto itPort = ports.begin();
        for (int i = 0; i < ips.size(); i++)
        {
            //nodesInfo.emplace_back(std::make_pair(*itIp, *itPort));
            //itPort++;
            itIp++;
        }
        return nodesInfo;
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "amount to use is grater then the amount of exisiting nodes!!!\n";
        return std::vector<std::pair<std::string, std::string>>();
    }
}