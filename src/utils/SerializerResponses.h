#pragma once 

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"
using json = nlohmann::json;

#define INIT_VEC_SIZE 4
//Node
class SerializerResponses
{
    public: 
        static std::vector<unsigned char> serializeResponse(const RsaKeyExchangeResponse& rsaKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse(const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse(const CircuitConfirmationResponse& circuitConfirmationResponse);
        static std::vector<unsigned char> serializeResponse(const HttpGetResponse& httpGetResponse);
       
};