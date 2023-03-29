#ifndef VIRTUAL_SENSOR_CMD_HANDLER_H
#define VIRTUAL_SENSOR_CMD_HANDLER_H


#include <nlohmann/json.hpp>
#include <handlers/ws_message.h>
#include <client_connector.h>

using json = nlohmann::json;

class CmdHandler {
public:
    virtual void handle(ClientConnector *responder, const WsMessage& msg) = 0;
    virtual ~CmdHandler() = default;
};


#endif //VIRTUAL_SENSOR_CMD_HANDLER_H
