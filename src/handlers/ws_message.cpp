#include <boost/format.hpp>
#include "handlers/ws_message.h"

WsMessage::WsMessage()
{
    // do nothing
}

WsMessage::WsMessage(WsMessage &&other)
{
    this->cmd = other.cmd;
    this->coordId = other.coordId;
    this->msg = other.msg;
    this->error = other.error;
    this->payload = other.payload;
}

WsMessage WsMessage::from(const WsMessage &other)
{
    WsMessage cp;
    cp.cmd = other.cmd;
    cp.coordId = other.coordId;
    return std::move(cp);
}

std::string WsMessage::toJson() const
{
    auto format = boost::format("{"
                                "\"cmd\": %d,"
                                "\"message\": \"%s\", "
                                "\"error\": %d, "
                                "\"coordId\": \"%s\","
                                "\"payload\": %s"
                                "}");
    return boost::str(
        format %
        this->cmd %
        this->msg %
        this->error %
        this->coordId %
        (this->payload ? this->payload->toJson() : "{}"));
}

WsMessage::~WsMessage()
{
    if (this->payload != nullptr)
    {
        delete this->payload;
    }
}
