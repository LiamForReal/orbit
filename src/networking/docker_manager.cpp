#include "docker_manager.h"

DockerManager::DockerManager()
{

}

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
    std::string command = "docker-compose up --build -d";  // -d flag runs it in detached mode

    for (int i = 1; i <= amount; i++)
    {
        command += " " + std::string(CONTAINER_NAME) + std::to_string(i); 
    }

    runCmdCommand("python ../dockerFiles/docker_ip_inishializer.py"); //pip install pyyaml - to run it
    runCmdCommand(command);
}

std::list<std::string> DockerManager::findIPs(const int& amount)
{
    std::list<std::string> nodesIp;
    char buffer[128];
    std::string containerID;
    
    for (int i = 0; i < amount; i++)
    {
        std::string containerIDCommand = "docker-compose ps -q";
        FILE* pipe = _popen(containerIDCommand.c_str(), "r");
        if (!pipe) throw std::runtime_error("Failed to run command");
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            containerID += buffer;
        }
        _pclose(pipe);
        containerID = containerID.substr(0, containerID.find("\n"));  // Clean up newlines
        std::string inspectCommand = "docker inspect -f \"{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}\" " + std::string(CONTAINER_NAME) + char(i + 49);
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


std::list<std::string> DockerManager::openAndGetIPs(const int& use, const int& create)
{
    openDocker(create);
    return findIPs(use);
}