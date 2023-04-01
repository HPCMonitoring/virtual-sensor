#ifndef VIRTUAL_SENSOR_WS_MESSAGE_H
#define VIRTUAL_SENSOR_WS_MESSAGE_H


#include <string>
#include <schema/json_serializable.h>

enum WsCommand {
    AUTH = 1000,
    SYS_INFO = 2000,
};

class WsMessage : public JsonSerializable {
public:
    WsCommand cmd;
    int error;
    std::string msg;
    std::string coordId;
    JsonSerializable *payload = nullptr;
    std::string toJson() const override;
    ~WsMessage() override;
};


#endif //VIRTUAL_SENSOR_WS_MESSAGE_H
