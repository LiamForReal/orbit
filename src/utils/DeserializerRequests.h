#pragma once

#include <iostream>
#include "Requestes.hpp"
#include "json.hpp"
using json = nlohmann::json;

#define INIT_VEC_SIZE 5
#define BYTES_TO_COPY 4
#define INC 1

//Node 

class DeserializerRequests
{
    public:
        static RsaKeyExchangeRequest deserializeRsaKeyExchangeRequest(const std::vector<unsigned char>& buffer);
        static EcdheKeyExchangeRequest deserializeEcdheKeyExchangeRequest(const std::vector<unsigned char>& buffer);
        static NodeOpenRequest deserializeNodeOpeningRequest(const std::vector<unsigned char>& buffer);
        static LinkRequest deserializeLinkRequest(const std::vector<unsigned char>& buffer);
        static HttpGetRequest deserializeHttpGetRequest(const std::vector<unsigned char>& buffer);
};



