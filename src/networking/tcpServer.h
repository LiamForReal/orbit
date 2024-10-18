#pragma once

#include "tcpConnection.hpp"
#include <map>
#include <vector>
#include <utility>

class tcp_server
{
    public:
        tcp_server(boost::asio::io_context& io_context)
            : io_context(io_context),
            acceptor(io_context, tcp::endpoint(tcp::v4(), 13)),
            is_client_being_served(false)
        {
            std::cout << "Server binding to port 13...\n";
            start_accept();
        }

    private:
        void start_accept()
        {
            if (!is_client_being_served) {
                std::cout << "Server waiting for connection...\n";
                tcp_connection::pointer new_connection = tcp_connection::create(io_context);
                acceptor.async_accept(new_connection->socket(),
                    std::bind(&tcp_server::handle_accept, this, new_connection,
                    boost::asio::placeholders::error));
            }
        }

        void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
        {
            if (!error)
            {
                std::cout << "Client connected.\n";
                is_client_being_served = true;  // Pause accepting other clients
                new_connection->start([this]() {
                    is_client_being_served = false;  // Resume accepting clients
                    start_accept();
                });
            }
            else
            {
                std::cerr << "Error accepting connection: " << error.message() << std::endl;
                start_accept();  // Resume accepting connections even on error
            }
        }

        boost::asio::io_context& io_context;
        tcp::acceptor acceptor;
        bool is_client_being_served;
};

/*
    std::map<int, tcp::socket> sockets;
    std::map<int, std::vector<tcp::socket>> paths;
    std::map<tcp::socket, std::chrono::high_resolution_clock::time_point> responseTime;
*/