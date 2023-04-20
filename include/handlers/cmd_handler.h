#ifndef __VIRTUAL_SENSOR_CMD_HANDLER_H__
#define __VIRTUAL_SENSOR_CMD_HANDLER_H__

#include "handlers/ws_message.h"
#include "client_connector.h"

using json = nlohmann::json;

class CmdHandler {
public:
    virtual void handle(ClientConnector *responder, const WsMessage& msg) = 0;
    virtual ~CmdHandler() = default;
};


#endif //VIRTUAL_SENSOR_CMD_HANDLER_H
