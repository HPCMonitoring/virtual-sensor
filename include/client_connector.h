#ifndef VIRTUAL_SENSOR_CLIENT_CONNECTOR_H
#define VIRTUAL_SENSOR_CLIENT_CONNECTOR_H
#include <handlers/ws_message.h>

class ClientConnector {
public:
    virtual void send(const WsMessage &res) = 0;
    virtual void onIdChange(std::string id) = 0;
};

#endif //VIRTUAL_SENSOR_CLIENT_CONNECTOR_H
