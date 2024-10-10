#include "server.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        // Creating a TCP acceptor to listen for incoming connections
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Server listening on port 12345..." << std::endl;

        // Waiting for a connection
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::cout << "Client connected!" << std::endl;

        // Buffer to hold the data
        std::string buffer;
        std::error_code error;

        // Receiving data from the client
        size_t len = socket.read_some(boost::asio::buffer(buffer), error);

        if (!error) {
            std::cout << "Received: " << std::string(buffer.data(), len) << std::endl;
        }

        // Sending a response back to the client
        std::string response = "Hello from Server!";
        boost::asio::write(socket, boost::asio::buffer(response), error);

        std::cout << "Response sent to client." << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
