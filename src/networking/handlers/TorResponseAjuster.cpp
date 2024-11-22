#include "TorResponseAjuster.h"

TorResponseAjuster::TorResponseAjuster() {}

NodeOpenResponse TorResponseAjuster::getNodeOpenResponse(const RequestInfo& requestInfo)
{
	CircuitConfirmationResponse ccr = DeserializerResponses::deserializeCircuitConfirmationResponse(requestInfo.buffer);

	for (auto it = ccr.nodesPath.begin(); it != ccr.nodesPath.end(); it++)
	{
		std::cout << "Node: " << it->first << " " << it->second << std::endl;
	}

}