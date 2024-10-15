#include "server.h"
#include <functional>
#include <iostream>
#include <ctime>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

int main()
{
    try
    {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13)); // 13 is the port

        for (;;)
        {
            std::array<char, 128> buf;
            boost::system::error_code error;

            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

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
        }
    }

    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

  return 0;
}