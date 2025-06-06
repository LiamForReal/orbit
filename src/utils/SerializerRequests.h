#pragma once

#include "json.hpp"
#include <iostream>
#include "Requestes.hpp"
using json = nlohmann::json;

#define INIT_VEC_SIZE 4
//client
 
class SerializerRequests
{
    public:
        static std::vector<unsigned char> serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest);
        static std::vector<unsigned char> serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest);
        static std::vector<unsigned char> serializeRequest(const NodeOpenRequest& nodeOpenRequest);
        static std::vector<unsigned char> serializeRequest(const LinkRequest& linkRequest);
        static std::vector<unsigned char> serializeRequest(const HttpGetRequest& httpGetRequest);
};