#include "docker_manager.h"

#include <random>

DockerManager::DockerManager()
{
    deleteDockerContainers();
    _clientsAmount = 0;
    this->amountCreated = 0;
    runCmdCommand("python ../../../dockerFiles/docker_node_info_init.py"); //pip install pyyaml - to run it
}


void DockerManager::deleteDockerContainers()
{
    std::vector<std::string> containers = runCommand("docker ps --format \"{{.Names}}\"");

    if (containers.empty()) {
        std::cout << "No running containers found." << std::endl;
    }
    else {
        for (const auto& container : containers) {
            std::cout << "Stopping and removing: " << container << std::endl;
            runCmdCommand("docker stop " + container);
            runCmdCommand("docker rm " + container);
        }
    }

    // Remove the specified Docker network
    runCmdCommand("docker network rm dockerfiles_TOR_NETWORK");
}

std::vector<std::string> DockerManager::runCommand(const std::string& command) 
{
    std::vector<std::string> result;
    char buffer[128];
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command" << std::endl;
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string containerName(buffer);
        containerName.erase(containerName.find_last_not_of(" \n\r") + 1); // Trim whitespace
        result.push_back(containerName);
    }
    _pclose(pipe);
    return result;
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
    if (this->amountCreated + create > NODE_CAPACITY)
        throw std::runtime_error("to many nodes the server cant allow it!");

    for (int i = 0; i < create; i++) //put all the nodes I wanna work with node1 - noden
        unDefinedNodes.emplace_back(std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1));
    //node2 node3 node4 , node1 node8 node6
    if (use > pathNodeExisting.size() + unDefinedNodes.size())//chack use confirmation
        throw std::runtime_error("use is grater then all the nodes exisiting");

}

std::vector<string> DockerManager::SelectPathAndAdjustNetwork(int use, const unsigned int circuitId)
{
    std::vector<string> nodeSelected;
    std::vector<string> tmp;
    for (int i = 0; i < unDefinedNodes.size(); ++i)  //node selected get all the nodes and choose unique paths to every client each
    {
        nodeSelected.emplace_back(unDefinedNodes[(i + _clientsAmount) % unDefinedNodes.size()]);
    }
    for (auto it : nodeSelected)
    {
        if (it != *nodeSelected.begin())
        {
            pathNodeExisting.emplace_back(it);
        }
        else guardNodeCircuits[circuitId] = it;
    }
    nodeSelected.clear();
    for (int i = 1; i < use; ++i)  //node selected get all the nodes and choose unique paths to every client each
    {
        tmp.emplace_back(pathNodeExisting[i % pathNodeExisting.size()]);
    }
    nodeSelected.emplace_back(guardNodeCircuits[circuitId]);
    nodeSelected.insert(nodeSelected.end(), tmp.begin(), tmp.end());
    string portsCommand = "python ../../../dockerFiles/adjust_ports_to_talk_in_subnet.py";
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
    std::string buildCommand = "cd ../../../dockerFiles/ && docker-compose -f Docker-compose.yaml up --build -d";
    for (int i = 0; i < amount; i++)
    {
        buildCommand += " " + std::string(CONTAINER_NAME) + std::to_string(this->amountCreated + i + 1);
    }
    runCmdCommand(buildCommand);
}

void DockerManager::openDocker(const string& containerName)
{
    if (containerName.empty())
    {
        std::cout << "Empty container name\n";
        return;
    }
    std::string buildCommand = "cd ../../../dockerFiles/ && docker-compose -f Docker-compose.yaml up -d " + containerName; //only restarts
    runCmdCommand(buildCommand);
}

std::vector<std::string> DockerManager::findIPs(std::vector<string>& containersNames)
{
    std::vector<std::string> nodesIp;
    std::string containerIDCommand = "cd ../../../dockerFiles/ && docker-compose ps -q";

    for (auto it : containersNames)
    {
        std::vector<std::string> containerIDs = runCommand(containerIDCommand);
        if (containerIDs.empty()) throw std::runtime_error("Failed to retrieve container ID");

        std::string containerID = containerIDs.front();
        std::string inspectCommand = "cd ../../../dockerFiles/ && docker inspect -f \"{{.NetworkSettings.Networks.dockerfiles_TOR_NETWORK.IPAMConfig.IPv4Address }}\" " + it;

        std::vector<std::string> containerIPs = runCommand(inspectCommand);
        if (containerIPs.empty()) throw std::runtime_error("Failed to retrieve container IP");

        nodesIp.push_back(containerIPs.front());
    }
    std::cout << "IPs found successfully!\n";
    return nodesIp;
}

std::vector<std::string> DockerManager::findControlPorts(std::vector<string>& containersNames)
{
    std::vector<std::string> controlNodesPorts;
    std::string inspectCommand;

    for (auto it : containersNames)
    {
        inspectCommand = "cd ../../../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9051/tcp\\\") 0).HostPort }}\" " + it;
        std::vector<std::string> portsStr = runCommand(inspectCommand);
        if (!portsStr.empty()) {
            try {
                std::string hostPort = std::to_string(std::stoi(portsStr.front()));
                std::cout << it << ": control port id is " << hostPort << "\n";
                controlNodesPorts.emplace_back(hostPort);
            }
            catch (const std::exception& ex) {
                std::cerr << "Error parsing port: " << ex.what() << std::endl;
            }
        }
    }
    return controlNodesPorts;
}


std::vector<std::string> DockerManager::findProxyPorts(std::vector<string>& containersNames)
{
    std::vector<std::string> proxyNodesPorts;
    if (containersNames.empty()) return proxyNodesPorts;

    std::string inspectCommand = "cd ../../../dockerFiles/ && docker inspect -f \"{{ (index (index .HostConfig.PortBindings \\\"9050/tcp\\\") 0).HostPort }}\" " + *containersNames.begin();
    std::vector<std::string> portsStr = runCommand(inspectCommand);
    if (!portsStr.empty()) {
        try {
            std::string hostPort = std::to_string(std::stoi(portsStr.front()));
            std::cout << "container 1 (of this function calling) proxy port is " << hostPort << "\n";
            proxyNodesPorts.emplace_back(hostPort);
            for (size_t i = 1; i < containersNames.size(); i++) {
                std::cout << "container " << std::to_string(i + 1) << " proxy port is 9050\n";
                proxyNodesPorts.emplace_back(INTERNAL_PORT);
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error parsing port: " << ex.what() << std::endl;
        }
    }
    return proxyNodesPorts;
}

std::vector<std::pair<std::string, std::string>> DockerManager::openAndGetInfo(const int& use, const int& create, const unsigned int circuitId)
{
    // 1 2 / 0 -> 2 - 1 1 > 0
    // 1 2 / 1 -> 2 - 1 1 = 1
    // 1 2 / 30 -> 2 - 1 1 < 30
    try
    {//3 
        if (create < use && use - create > this->amountCreated)
        {
            throw std::runtime_error("user input problem");
        }
        _clientsAmount++;
        std::vector<std::pair<std::string, std::string>> nodesInfo;
        setNewNodes(create, use);
        std::vector<string> nodeSelected = SelectPathAndAdjustNetwork(use, circuitId);
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
        throw;
    }
}

std::vector<std::pair<std::string, std::string>> DockerManager::GetControlInfo() //for server
{
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<string> nodeExisting;
    std::vector<string> guardNodeExisting;

    for (auto it : this->guardNodeCircuits)
        guardNodeExisting.emplace_back(it.second);

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

/*
* 1find the node name -> 2reconfigure its ip by py script ->
* 3rerun him -> 4add him to the nodes properly path/guard ->
* use function to find circuit -> get circuit ips and ports -> return
*/
std::vector<std::pair<std::string, std::string>> DockerManager::giveCircuitAfterCrush(string NodeIp, const int use, const unsigned int circuitId) // list of ips
{
    try
    {
        std::vector<std::pair<std::string, std::string>> nodesInfo;
        std::vector<string> nodes;
        string nodeName = "";
        int NodeNumber = 1;

        for (int i = 1; i <= NODE_CAPACITY; i++)
            nodes.emplace_back(CONTAINER_NAME + std::to_string(i));
        std::vector<std::string> ips = findIPs(nodes);
        for (auto it : ips)
        {
            if (NodeIp == it)
            {
                nodeName = CONTAINER_NAME + std::to_string(NodeNumber);
                std::cout << "Found that node container name is " << nodeName << " (" << NodeIp << ")" << std::endl;
                break;
            }
            NodeNumber++;
        }
        std::string regenerateIpsCommend = "python ../../../dockerFiles/docker_node_ip_changer.py " + nodeName;
        runCmdCommand(regenerateIpsCommend);
        std::vector<string> nodeSelected = SelectPathAndAdjustNetwork(use, circuitId);
        std::vector<std::string> ports = findProxyPorts(nodeSelected);
        openDocker(nodeName);
        ips = findIPs(nodeSelected);
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
    catch (std::runtime_error& e)
    {
        std::cerr << "amount to use is grater then the amount of exisiting nodes!!!\n";
        return std::vector<std::pair<std::string, std::string>>();
    }
}