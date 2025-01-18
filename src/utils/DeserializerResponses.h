#pragma once

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"

using json = nlohmann::json;

#define INIT_VEC_SIZE 4
//client 

class DeserializerResponses 
{
    public: 
        static RsaKeyExchangeResponse deserializeRsaKeyExchangeResponse(const std::vector<unsigned char>& buffer);
        static EcdheKeyExchangeResponse deserializeEcdheKeyExchangeResponse(const std::vector<unsigned char>& buffer);
        static NodeOpenResponse deserializeNodeOpeningResponse(const std::vector<unsigned char>& buffer);
        static CircuitConfirmationResponse deserializeCircuitConfirmationResponse(const std::vector<unsigned char>& buffer);
        static LinkResponse deserializeLinkResponse(const std::vector<unsigned char>& buffer);
        static HttpGetResponse deserializeHttpGetResponse(const std::vector<unsigned char>& buffer);
        static CloseConnectionResponse deserializeCloseConnectionResponse(const std::vector<unsigned char>& buffer);
        static DeleteCircuitResponse deserializeDeleteCircuitResponse(const std::vector<unsigned char>& buffer);        
};