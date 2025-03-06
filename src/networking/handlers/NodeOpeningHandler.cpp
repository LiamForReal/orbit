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
    unsigned int status = CIRCUIT_CONFIRMATION_STATUS, circuitId = 0;
    try
    {
        requestInfo.buffer = _aes.decrypt(requestInfo.buffer);
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(requestInfo);

        std::cout << "client sent: " << requestInfo.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;
        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open, this->circuit_id);
      
        controlNodesInfo = dm.GetControlInfo();

        circuitId = this->circuit_id;
        ccr.nodesPath = nodesInfo;
        this->_controlList[circuitId] = controlNodesInfo;

        std::cout << "\n\nthe circuit chosen is " << circuitId << "\n\n";
        _clients[circuitId] = INVALID_SOCKET;
        while (_controlList.find(this->circuit_id) != _controlList.end())
        {
            this->circuit_id = (this->circuit_id + 1) % 255;
        }
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
        status = CIRCUIT_CONFIRMATION_ERROR;
    }
    std::vector<unsigned char> data = SerializerResponses::serializeResponse(ccr);
    data = _aes.encrypt(data);
    rr.buffer = Helper::buildRR(data, status, data.size(), circuitId);
    return rr;
}