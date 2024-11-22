#pragma once

#include "../../utils/DeserializerResponses.h"
#include "../../utils/RequestInfo.hpp"

class TorResponseAjuster
{
public:
	TorResponseAjuster();
	NodeOpenResponse getNodeOpenResponse(const RequestInfo& requestInfo);//more...
};