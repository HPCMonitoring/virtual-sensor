#ifndef __VIRTUAL_SENSOR_CLIENT_CONNECTOR_H__
#define __VIRTUAL_SENSOR_CLIENT_CONNECTOR_H__
#include "handlers/ws_message.h"

class ClientConnector {
public:
    virtual void send(const WsMessage &res) = 0;
    virtual void onIdChange(std::string id) = 0;
    virtual std::unordered_map<std::string, std::string> getMetadata() = 0;
};

#endif //VIRTUAL_SENSOR_CLIENT_CONNECTOR_H
