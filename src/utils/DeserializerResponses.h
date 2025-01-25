#pragma once

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"
#include "RequestInfo.hpp"
using json = nlohmann::json;

#define INIT_VEC_SIZE 4
//client 

class DeserializerResponses 
{
    public: 
        static RsaKeyExchangeResponse deserializeRsaKeyExchangeResponse(const RequestInfo ri);
        static EcdheKeyExchangeResponse deserializeEcdheKeyExchangeResponse(const RequestInfo ri);
        static CircuitConfirmationResponse deserializeCircuitConfirmationResponse(const RequestInfo ri);
        static HttpGetResponse deserializeHttpGetResponse(const RequestInfo ri);   
};