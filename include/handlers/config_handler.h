#ifndef VIRTUAL_SENSOR_CONFIG_HANDLER_H
#define VIRTUAL_SENSOR_CONFIG_HANDLER_H

#include <list>
#include "recorder.h"
#include "ws_message.h"
#include "client_connector.h"
#include "cmd_handler.h"

class ConfigHandler : public CmdHandler {
private:
    std::list<Recorder::Worker> workers;
    void handle(ClientConnector *responder, const WsMessage& msg) override;
};
#endif //VIRTUAL_SENSOR_CONFIG_HANDLER_H
