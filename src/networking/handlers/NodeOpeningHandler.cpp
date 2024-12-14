#include "NodeOpeningHandler.h"
#include <algorithm>
unsigned int NodeOpeningHandler::circuit_id = 1;

NodeOpeningHandler::NodeOpeningHandler(DockerManager& dockerManager, std::map<unsigned int, std::list<std::pair<std::string, std::string>>>& controlList) 
    : dm(dockerManager), _controlList(controlList)
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
    std::list<std::pair<std::string, std::string>> controlNodesInfo;
    CircuitConfirmationResponse ccr;
    this->rr.buffer.clear();

    try
    {
        NodeOpenRequest nor = DeserializerRequests::deserializeNodeOpeningRequest(requestInfo.buffer);

        std::cout << "client sent: " << requestInfo.id << "\nbuffer(open): " << nor.amount_to_open << "\nbuffer(use): " << nor.amount_to_use << std::endl;

        // here open and get ips from docker.
        nodesInfo = dm.openAndGetInfo(nor.amount_to_use, nor.amount_to_open);
        controlNodesInfo = dm.GetControlInfo();
        ccr.status = Status::CIRCUIT_CONFIRMATION_STATUS;

        for (auto it = nodesInfo.begin(); it != nodesInfo.end(); it++)
        {
            ccr.nodesPath.emplace_back(*it);
        }
        /*
        * HERE'S WHAT THAT 
        NEED TO BUILD THE MAP FROM circuit id TO list of nodes in the circuit
        * YOU SOULD PASS BY REFRENCE THE MAP AND AJUST IT IN THE FNCTION
        */

        this->_controlList[this->circuit_id] = controlNodesInfo;

        ccr.circuit_id = this->circuit_id;
        this->circuit_id++;
    }
    catch (std::runtime_error e)
    {
        ccr.status = Errors::CIRCUIT_CONFIRMATION_ERROR;
    }

    rr.buffer = SerializerResponses::serializeResponse(ccr);

    return rr;
}

unsigned int NodeOpeningHandler::getCircuitID()
{
    return circuit_id;
}
