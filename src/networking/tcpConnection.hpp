#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context& io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start(std::function<void()> on_finish)
    {
        std::cout << "Connection established. Waiting for message from client...\n";
        
        // Asynchronously wait for data from the client
        boost::asio::async_read(socket_, boost::asio::buffer(message_),
            std::bind(&tcp_connection::handle_read, shared_from_this(),
                      std::placeholders::_1, 
                      std::placeholders::_2, 
                      on_finish));  
    }

private:
    tcp_connection(boost::asio::io_context& io_context)
        : socket_(io_context)
    {
    }

    // Handle the completion of a read operation
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, std::function<void()> on_finish)
    {
        if (!error)
        {
            // Successfully read a message from the client
            std::string client_message(message_.data(), bytes_transferred);
            std::cout << "Message received from client: " << client_message << "\n";


            std::string response = "hello from server";

    
            boost::asio::async_write(socket_, boost::asio::buffer(response),
                std::bind(&tcp_connection::handle_write, shared_from_this(),
                          std::placeholders::_1,  
                          std::placeholders::_2, 
                          on_finish));  
        }
        else
        {
            std::cerr << "Error during async_read: " << error.message() << std::endl;      
        }
        on_finish();  // Call on_finish in case of an error      

    }
    
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred, std::function<void()> on_finish)
    {
        if (!error)
        {
            std::cout << "Message sent successfully (" << bytes_transferred << " bytes).\n";
        }
        else
        {
            std::cerr << "Error during write: " << error.message() << std::endl;
        }
        on_finish();
    }

    tcp::socket socket_;
    std::array<char, 128> message_;  // Fixed-size buffer for reading
};