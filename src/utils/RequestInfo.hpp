#pragma once
#include <vector>

typedef struct RequestInfo
{
    unsigned int id;
    unsigned int circuit_id;
    std::vector<unsigned char> buffer;
    RequestInfo()
    {
        this->id = 0;
        this->buffer.clear();
    }
} RequestInfo;