#ifndef __VIRTUAL_SENSOR_CONFIG_HANDLER_H__
#define __VIRTUAL_SENSOR_CONFIG_HANDLER_H__

#include "clients/kafka_client.h"
#include "ws_message.h"
#include "client_connector.h"
#include "cmd_handler.h"

class ConfigHandler : public CmdHandler {
private:
    std::list<KakfaClient::Worker> workers;
    std::shared_ptr<spdlog::logger> _logger;
public:
    ConfigHandler();
    void handle(ClientConnector *responder, const WsMessage& msg) override;
};
#endif
