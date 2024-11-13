#pragma once 

#include <iostream>
#include "Responses.hpp"
#include "json.hpp"
using json = nlohmann::json;

#define INIT_VEC_SIZE 5
#define BYTES_TO_COPY 4
#define INC 1
//server

class SerializerResponses
{
    public: 
        static std::vector<unsigned char> serializeResponse(const RsaKeyExchangeResponse& rsaKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse(const EcdheKeyExchangeResponse& ecdheKeyExchangeResponse);
        static std::vector<unsigned char> serializeResponse(const NodeOpenResponse& nodeOpenResponse);
        static std::vector<unsigned char> serializeResponse(const CircuitConfirmationResponse& circuitConfirmationResponse);
        static std::vector<unsigned char> serializeResponse(const LinkResponse& linkResponse);
        static std::vector<unsigned char> serializeResponse(const HttpGetResponse& httpGetResponse);
        static std::vector<unsigned char> serializeResponse(const CloseConnectionResponse& closeConnectionResponse);
        static std::vector<unsigned char> serializeResponse(const DeleteCircuitResponse& deleteCircuitResponse);
        static std::vector<unsigned char> serializeResponse(const AliveMsgResponse& aliveMsgResponse);
        static std::vector<unsigned char> serializeResponse(const GetDomainResponse& getDomainResponse);

};