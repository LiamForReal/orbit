#include <iostream>

int main()
{
	int result;
	try
	{
		result = system("Node.exe");
		if (result == 0) 
			std::cout << "Docker Compose process started successfully." << std::endl;
		else std::cerr << "Failed to start Docker. Error code: " << result << std::endl;
	}
	catch(const std::runtime_error& e)
	{
		std::cout << e << std::endl;
	}
	
	return 0;
}