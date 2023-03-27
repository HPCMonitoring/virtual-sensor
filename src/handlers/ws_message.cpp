#include <boost/format.hpp>
#include "handlers/ws_message.h"

WsMessage::~WsMessage() {
    if (this->payload != nullptr) {
        delete this->payload;
    }
}

std::string WsMessage::toJson() const {
    auto format = boost::format("{"
                                "message: %s, "
                                "error: %d, "
                                "payload: %s,"
                                "}");
    return boost::str(
            format %
            this->msg %
            this->error %
            this->payload->toJson()
    );
}
