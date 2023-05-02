#ifndef __VIRTUAL_SENSOR_CONFIG_HANDLER_H__
#define __VIRTUAL_SENSOR_CONFIG_HANDLER_H__

#include <nlohmann/json.hpp>
#include "cmd_handler.h"
#include "clients/kafka_client.h"
#include <list>

class ConfigHandler : public CmdHandler {
private:
    std::list<KakfaClient::Worker> workers;
    std::shared_ptr<spdlog::logger> _logger;
public:
    ConfigHandler();
    void handle(ClientConnector *responder, const WsMessage& msg) override;
};
#endif
