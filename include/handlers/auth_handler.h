#ifndef VIRTUAL_SENSOR_AUTH_HANDLER_H
#define VIRTUAL_SENSOR_AUTH_HANDLER_H

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
