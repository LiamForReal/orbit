#pragma once
#include <iostream>
#include <vector>

typedef struct RequestInfo
{
    unsigned int id;
    unsigned int circuit_id;
    std::vector<unsigned char> buffer;
} RequestInfo;

typedef struct RequestResult // Changed from typedef to struct definition
{
    std::vector<unsigned char> buffer;
} RequestResult;


class IRequestHandler
{
    public:
        virtual bool isRequestRelevant(const RequestInfo& requestInfo) = 0;
        virtual RequestResult handleRequest(const RequestInfo& requestInfo) = 0;
};
