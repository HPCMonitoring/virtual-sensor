#include <boost/format.hpp>
#include "handlers/ws_message.h"

WsMessage::~WsMessage() {
    if (this->payload != nullptr) {
        delete this->payload;
    }
}

std::string WsMessage::toJson() const {
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
            this->payload->toJson()
    );
}
