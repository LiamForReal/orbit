#pragma once
#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include "../../utils/Requestes.hpp"
#include "../docker_manager.h"
#include "../../utils/SerializerResponses.h"
#include "../../utils/DeserializerRequests.h"
#include "../../utils/SerializerRequests.h"
#include "../../utils/DeserializerResponses.h"
#include "../../utils/Responses.hpp"
#include "../../utils/RequestInfo.hpp"
#include "../../utils/Helper.h"

typedef struct RequestResult // Changed from typedef to struct definition
{
    std::vector<unsigned char> buffer;
} RequestResult;


class IRequestHandler
{
public:
    virtual bool isRequestRelevant(const RequestInfo& requestInfo) = 0;
    virtual RequestResult handleRequest(RequestInfo& requestInfo) = 0;
};
