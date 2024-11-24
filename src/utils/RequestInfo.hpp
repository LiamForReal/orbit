#pragma once
#include <vector>

typedef struct RequestInfo
{
    unsigned int id;
    std::vector<unsigned char> buffer;
} RequestInfo;