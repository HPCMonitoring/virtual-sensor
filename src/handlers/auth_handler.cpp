#include "handlers/auth_handler.h"

AuthHandler::AuthHandler() {
    this->_logger = SensorLogger::getInstance()->getLogger();
}

void AuthHandler::handle(ClientConnector *responder, const WsMessage& msg) {
    if (msg.error == 0) {
        std::stringstream ss;
        ss << "auth successfully with message: " << msg.toJson();
        SPDLOG_LOGGER_INFO(
                this->_logger,
                ss.str());
        json msgJson = json::parse(msg.payload->toJson());
        responder->onIdChange(msgJson["id"]);
    }
    else {
        std::stringstream ss;
        ss << "auth failed with message: " << msg.toJson();
        SPDLOG_LOGGER_ERROR(
                this->_logger,
                ss.str());
    }
}
