#ifndef __VIRTUAL_SENSOR_WS_MESSAGE_H__
#define __VIRTUAL_SENSOR_WS_MESSAGE_H__

#include "schemas/json_serializable.h"
#include <boost/format.hpp>

enum WsCommand {
    AUTH = 1000,
    SYS_INFO = 2000,
    CONFIG = 2001
};

enum WsErrorCode {
    SUCCESS = 0,
    UNKNOWN = 999,
};

class WsMessage : public JsonSerializable {
public:
    WsCommand cmd;
    int error;
    std::string msg;
    std::string coordId;
    JsonSerializable *payload = nullptr;

public:
    static WsMessage from(const WsMessage &);
    std::string toJson() const override;
    ~WsMessage() override;
    WsMessage();
    WsMessage(WsMessage &&);
    WsMessage(WsMessage &) = delete;
    void operator=(const WsMessage &) = delete;
};


#endif //VIRTUAL_SENSOR_WS_MESSAGE_H
