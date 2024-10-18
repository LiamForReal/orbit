#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "tcpConnection.hpp"
#include <map>
#include <vector>
#include <utility>
using boost::asio::ip::tcp;

class tcp_server
{
public:
    tcp_server(boost::asio::io_context& io_context)
        : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
    {
        std::cout << "Server binding to port 13...\n";
        start_accept();
    }
private:
    void start_accept()
    {
        std::cout << "Server waiting for connection...\n";
        tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
        acceptor_.async_accept(new_connection->socket(),
            std::bind(&tcp_server::handle_accept, this, new_connection,
            boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "Client connected.\n";
            new_connection->start();
        }
        else
        {
            std::cerr << "Error accepting connection: " << error.message() << std::endl;
        }
        start_accept(); // Continue accepting connections
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
};

/*  
    std::map<int, tcp::socket> sockets;
    std::map<int, std::vector<tcp::socket> paths; 
    std::map<tcp::socket, std::chrono::high_resolution_clock::time_point> responseTime;
*/