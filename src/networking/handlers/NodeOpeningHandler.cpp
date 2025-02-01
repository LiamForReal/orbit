#include "NodeOpeningHandler.h"
#include <algorithm>

unsigned int NodeOpeningHandler::circuit_id = 1; //256 so take a space from there

NodeOpeningHandler::NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients, AES& aes)
    : dm(dockerManager), _controlList(controlList), _clients(clients), _aes(aes)
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
    RequestInfo ri;
    this->rr.buffer.clear();
    unsigned int status = CIRCUIT_CONFIRMATION_STATUS;
    try
    {
        ri = requestInfo;
        ri.buffer = _aes.decrypt(ri.buffer);
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(ri);

        std::cout << "client sent: " << ri.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;

        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open, this->circuit_id);
        if (nodesInfo.empty())
            throw std::runtime_error("the failed to take nodes details");
        controlNodesInfo = dm.GetControlInfo();

        ccr.nodesPath = nodesInfo;
        this->_controlList[this->circuit_id] = controlNodesInfo;

        std::cout << "\n\nthe circuit chosen is " << circuit_id << "\n\n";
        _clients[circuit_id] = INVALID_SOCKET;
    }
    catch (std::runtime_error e)
    {
        status = CIRCUIT_CONFIRMATION_ERROR;
    }
    std::vector<unsigned char> data = SerializerResponses::serializeResponse(ccr);
    data = _aes.encrypt(data);
    rr.buffer = Helper::buildRR(data, status, data.size(), this->circuit_id);
    this->circuit_id++;
    return rr;
}