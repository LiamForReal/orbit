#include "node.h"

int main()
{
    std::cout << "Running Docker container...";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    getchar();
    return 0;
}