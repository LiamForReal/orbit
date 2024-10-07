#pragma once
#include <../../boost/asio/basic_raw_socket.hpp>

namespace asio = boost::asio;

class Server 
{
    public:
        Server();
        ~Server();

        void run();

    private:
};