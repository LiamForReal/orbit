#include "tcpServer.h"

int main()
{
    try
    {
        boost::asio::io_context io_context;
        std::cout << "Starting server...\n";
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}