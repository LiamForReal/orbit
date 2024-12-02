#include "NodeOpeningHandler.h"
unsigned int NodeOpeningHandler::circuit_id = 1;

NodeOpeningHandler::NodeOpeningHandler(DockerManager& dockerManager) : dm(dockerManager) 
{ 
    this->rr = RequestResult(); 
}

bool NodeOpeningHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return requestInfo.id == NODE_OPEN_RC;
}

RequestResult NodeOpeningHandler::handleRequest(const RequestInfo& requestInfo)
{
    std::list<std::pair<std::string, std::string>> nodesInfo;
    CircuitConfirmationResponse ccr;
    this->rr.buffer.clear();
    try
    {
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(requestInfo.buffer);

        std::cout << "client sent: " << requestInfo.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;

        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open);

        ccr.status = Status::CIRCUIT_CONFIRMATION_STATUS;

        for (auto it = nodesInfo.begin(); it != nodesInfo.end(); it++)
        {
            ccr.nodesPath.emplace_back(*it);
        }

        ccr.circuit_id = this->circuit_id;
        this->circuit_id++;
    }
    catch(std::runtime_error e)
    {
        ccr.status = Errors::CIRCUIT_CONFIRMATION_ERROR;
    }

    rr.
        = SerializerResponses::serializeResponse(ccr);

	return rr;
}
