#pragma once

#include <iostream>
#include "Requestes.hpp"
#include "json.hpp"
using json = nlohmann::json;
#include "RequestInfo.hpp"
#define INIT_VEC_SIZE 4

//Node 

class DeserializerRequests
{
    public:
        static RsaKeyExchangeRequest deserializeRsaKeyExchangeRequest(const RequestInfo ri);
        static EcdheKeyExchangeRequest deserializeEcdheKeyExchangeRequest(const RequestInfo ri);
        static NodeOpenRequest deserializeNodeOpeningRequest(const RequestInfo ri);
        static LinkRequest deserializeLinkRequest(const RequestInfo ri);
        static HttpGetRequest deserializeHttpGetRequest(const RequestInfo ri);
};



