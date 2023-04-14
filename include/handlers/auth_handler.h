#ifndef __VIRTUAL_SENSOR_AUTH_HANDLER_H__
#define __VIRTUAL_SENSOR_AUTH_HANDLER_H__

#include "cmd_handler.h"
#include "sensor_logger.h"

class AuthHandler : public CmdHandler {
private:
    std::shared_ptr<spdlog::logger> _logger;
public:
    AuthHandler();
    void handle(ClientConnector *responder, const WsMessage& msg) override;
};

#endif //VIRTUAL_SENSOR_AUTH_HANDLER_H
