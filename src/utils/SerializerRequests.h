#pragma once

#include "json.hpp"
#include <iostream>
#include "Requestes.hpp"
using json = nlohmann::json;
//client
 
class SerializerRequests
{
    public:
        static std::vector<unsigned char> serializeRequest(const RsaKeyExchangeRequest& rsaKeyExchangeRequest);
        static std::vector<unsigned char> serializeRequest(const EcdheKeyExchangeRequest& ecdheKeyExchangeRequest);
        static std::vector<unsigned char> serializeRequest(const NodeOpenRequest& nodeOpenRequest);
        static std::vector<unsigned char> serializeRequest(const LinkRequest& linkRequest);
        static std::vector<unsigned char> serializeRequest(const HttpGetRequest& httpGetRequest);
        static std::vector<unsigned char> serializeRequest(const CloseConnectionRequest& closeConnectionRequest);
        static std::vector<unsigned char> serializeRequest(const DeleteCircuitRequest& deleteCircuitRequest);
        static std::vector<unsigned char> serializeRequest(const AliveMsgRequest& aliveMsgRequest);
};