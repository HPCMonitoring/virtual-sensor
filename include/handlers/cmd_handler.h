#ifndef VIRTUAL_SENSOR_CMD_HANDLER_H
#define VIRTUAL_SENSOR_CMD_HANDLER_H


#include <nlohmann/json.hpp>
#include "ws_message.h"
#include <client_connector.h>

using json = nlohmann::json;

enum WsCommand {
    AUTH = 1000,
    SYS_INFO = 1001,
};

class CmdHandler {
public:
    virtual void handle(ClientConnector *responder, const WsMessage& msg) = 0;
    virtual ~CmdHandler() = default;
};


#endif //VIRTUAL_SENSOR_CMD_HANDLER_H
