#include "NodeOpeningHandler.h"
#include <algorithm>

unsigned int NodeOpeningHandler::circuit_id = 1; //256 so take a space from there

NodeOpeningHandler::NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients)
    : dm(dockerManager), _controlList(controlList), _clients(clients)
{
    srand(time(NULL));
    NodeOpeningHandler::circuit_id = 1 + (rand() % 200);
    this->rr = RequestResult();
}

bool NodeOpeningHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
    return requestInfo.id == NODE_OPEN_RC;
}

RequestResult NodeOpeningHandler::handleRequest(const RequestInfo& requestInfo)
{
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<std::pair<std::string, std::string>> controlNodesInfo;
    CircuitConfirmationResponse ccr;
    this->rr.buffer.clear();

    try
    {
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(requestInfo.buffer);

        std::cout << "client sent: " << requestInfo.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;

        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open, this->circuit_id);
        if (nodesInfo.empty())
            throw std::runtime_error("the failed to take nodes details");
        controlNodesInfo = dm.GetControlInfo();
        ccr.status = Status::CIRCUIT_CONFIRMATION_STATUS;

        ccr.nodesPath = nodesInfo;
        this->_controlList[this->circuit_id] = controlNodesInfo;

        std::cout << "\n\nthe circuit chosen is " << circuit_id << "\n\n";
        _clients[circuit_id] = INVALID_SOCKET;
    }
    catch (std::runtime_error e)
    {
        ccr.status = CIRCUIT_CONFIRMATION_ERROR;
    }
    rr.buffer = Helper::buildRR(SerializerResponses::serializeResponse(ccr), ccr.status, this->circuit_id);
    this->circuit_id++;
    return rr;
}
//get last id