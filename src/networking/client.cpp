#include "client.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        // Creating a TCP socket and resolving the server's address
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Sending data to the server
        std::string request = "Hello from Client!";
        boost::asio::write(socket, boost::asio::buffer(request));

        std::cout << "Sent to server: " << request << std::endl;

        // Buffer to hold the server's response
        std::string buffer;
        std::error_code error;

        // Receiving the server's response
        size_t len = socket.read_some(boost::asio::buffer(buffer), error);

        if (!error) {
            std::cout << "Received from server: " << std::string(buffer.data(), len) << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}