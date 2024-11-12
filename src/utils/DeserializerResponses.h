#pragma once

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"

using json = nlohmann::json;
//client 

class DeserializerResponses 
{
    public: 
        static RsaKeyExchangeResponse deserializeRsaKeyExchangeResponse(const std::vector<unsigned char>& buffer);
        static EcdheKeyExchangeResponse deserializeEcdheKeyExchangeResponse(const std::vector<unsigned char>& buffer);
        static NodeOpenResponse deserializeNodeOpeningResponse(const std::vector<unsigned char>& buffer);
        static LinkResponse deserializeLinkResponse(const std::vector<unsigned char>& buffer);
        static HttpGetResponse deserializeHttpGetResponse(const std::vector<unsigned char>& buffer);
        static CloseConnectionResponse deserializeCloseConnectionResponse(const std::vector<unsigned char>& buffer);
        static DeleteCircuitResponse deserializeDeleteCircuitResponse(const std::vector<unsigned char>& buffer);
        static GetDomainResponse deserializeGetDomainResponse(const std::vector<unsigned char>& buffer);
        
};