#include "client.h"
#include <iostream>
#include <array>
#include <boost/asio.hpp>

namespace ip = boost::asio::ip;
using ip::tcp;

int main(int argc, char* argv[])
{
    try
    { 
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "13");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        for (;;)
        {
            std::array<char, 128> buf;
            boost::system::error_code error;

            std::string message = "HaShem is the G-d!";

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
            {
                break; // Connection closed cleanly by peer.
            }
            else if (error)
            {
                throw boost::system::system_error(error); // Some other error.
            }

            std::cout.write(buf.data(), len);

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    getchar();
    return 0;
}