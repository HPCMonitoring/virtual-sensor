#include "handlers/ws_message.h"
#include <sstream>

WsMessage::WsMessage() {
    // do nothing
}

WsMessage::WsMessage(WsMessage &&other) {
    this->cmd = other.cmd;
    this->coordId = other.coordId;
    this->msg = other.msg;
    this->error = other.error;
    this->payload = other.payload;
}

WsMessage WsMessage::from(const WsMessage &other) {
    WsMessage cp;
    cp.cmd = other.cmd;
    cp.coordId = other.coordId;
    return std::move(cp);
}

std::string WsMessage::toJson() const {
    std::stringstream ss;

    ss    << "{"
          << "\"cmd\":"     <<          this->cmd       << ","
          << "\"message\":" << "\"" <<  this->msg       << "\"" << ","
          << "\"error\":"   <<          this->error     << ","
          << "\"coordId\":" <<          this->coordId   << ","
          << "\"payload\":" << "\"" <<  (this->payload ? this->payload->toJson() : "{}") << "\""
          << "}";
    return ss.str();
}

WsMessage::~WsMessage() {
    if (this->payload != nullptr) {
        delete this->payload;
    }
}
