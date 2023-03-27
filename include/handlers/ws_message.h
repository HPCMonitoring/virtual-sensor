#ifndef VIRTUAL_SENSOR_WS_MESSAGE_H
#define VIRTUAL_SENSOR_WS_MESSAGE_H


#include <string>
#include "schema/json_serializable.h"

class WsMessage : public JsonSerializable {
public:
    int error;
    std::string msg;
    JsonSerializable *payload = nullptr;
    std::string toJson() const override;
    ~WsMessage() override;
};


#endif //VIRTUAL_SENSOR_WS_MESSAGE_H
