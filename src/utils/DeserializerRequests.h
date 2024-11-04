#pragma once

#include <iostream>
#include "Requestes.hpp"
#include "json.hpp"
using json = nlohmann::json;

//server 

class DeserializerRequests
{
    public:
        static RsaKeyExchangeRequest deserializeRsaKeyExchangeRequest(const std::vector<unsigned char>& buffer);
        static EcdheKeyExchangeRequest deserializeEcdheKeyExchangeRequest(const std::vector<unsigned char>& buffer);
        static NodeOpenRequest deserializeNodeOpeningRequest(const std::vector<unsigned char>& buffer);
        static LinkRequest deserializeLinkRequest(const std::vector<unsigned char>& buffer);
        static HttpGetRequest deserializeHttpGetRequest(const std::vector<unsigned char>& buffer);
        static CloseConnectionRequest deserializeCloseConnectionRequest(const std::vector<unsigned char>& buffer);
        static DeleteCircuitRequest deserializeDeleteCircuitRequest(const std::vector<unsigned char>& buffer);
};



