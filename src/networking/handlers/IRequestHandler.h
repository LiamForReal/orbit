#pragma once
#include <iostream>
#include <vector>
#include "../../utils/Requestes.hpp"
#include "../docker_manager.h"
#include "../../utils/SerializerResponses.h"
#include "../../utils/DeserializerRequests.h"
#include "../../utils/Responses.hpp"


typedef struct RequestResult // Changed from typedef to struct definition
{
    std::vector<unsigned char> buffer;
} RequestResult;


class IRequestHandler
{
    public:
        virtual bool isRequestRelevant(const RequestInfo& requestInfo) = 0;
        virtual RequestResult handleRequest(const RequestInfo& requestInfo, DockerManager dm) = 0;
};
