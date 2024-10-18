#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>
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

  void start()
  {
    message_ = "hello for server";
    std::cout << "Connection established. Sending message...\n";

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        std::bind(&tcp_connection::handle_write, shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  tcp_connection(boost::asio::io_context& io_context)
    : socket_(io_context)
  {
  }

  void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {
    std::cout << "Message sent successfully.\n";
  }

  tcp::socket socket_;
  std::string message_;
};