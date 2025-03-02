#include "NodeOpeningHandler.h"
#include <algorithm>

unsigned int NodeOpeningHandler::circuit_id = 1; //256 so take a space from there

NodeOpeningHandler::NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::vector<std::pair<std::string, std::string>>>& controlList, std::map<unsigned int, SOCKET>& clients, AES& aes)
    : dm(dockerManager), _controlList(controlList), _clients(clients), _aes(aes)
{
    srand(time(NULL));
    NodeOpeningHandler::circuit_id = 1 + (rand() % 255);
    this->rr = RequestResult();
}

bool NodeOpeningHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
    return requestInfo.id == NODE_OPEN_RC;
}

RequestResult NodeOpeningHandler::handleRequest(RequestInfo& requestInfo)
{
    std::vector<std::pair<std::string, std::string>> nodesInfo;
    std::vector<std::pair<std::string, std::string>> controlNodesInfo;
    CircuitConfirmationResponse ccr;
    this->rr.buffer.clear();
    unsigned int status = CIRCUIT_CONFIRMATION_STATUS;
    unsigned int circuit_id = requestInfo.circuit_id;

    if (_controlList.find(circuit_id) != _controlList.end() && requestInfo.length == 0) //the circuit already exsisting in case of crush 
    {
        ccr.nodesPath = _controlList[circuit_id];
        std::vector<unsigned char> data = SerializerResponses::serializeResponse(ccr);
        data = _aes.encrypt(data);
        rr.buffer = Helper::buildRR(data, status, data.size(), circuit_id);
        return rr;
    }

    try
    {
        requestInfo.buffer = _aes.decrypt(requestInfo.buffer);
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(requestInfo);
        std::cout << "client sent: " << requestInfo.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;
        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open, this->circuit_id);
        if (nodesInfo.empty())
            throw std::runtime_error("the failed to take nodes details");
        controlNodesInfo = dm.GetControlInfo();

        ccr.nodesPath = nodesInfo;
        this->_controlList[this->circuit_id] = controlNodesInfo;

        std::cout << "\n\nthe circuit chosen is " << this->circuit_id << "\n\n";
        _clients[this->circuit_id] = INVALID_SOCKET;
    }
    catch (std::runtime_error e)
    {
        status = CIRCUIT_CONFIRMATION_ERROR;
    }
    std::vector<unsigned char> data = SerializerResponses::serializeResponse(ccr);
    data = _aes.encrypt(data);
    rr.buffer = Helper::buildRR(data, status, data.size(), this->circuit_id);
    this->circuit_id = (this->circuit_id + 1) % 255;
    while (_controlList.find(this->circuit_id) != _controlList.end())
    {
        this->circuit_id = (this->circuit_id + 1) % 255;
    }
    return rr;
}