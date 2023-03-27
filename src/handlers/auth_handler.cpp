#include "handlers/auth_handler.h"
#include <boost/format.hpp>

AuthHandler::AuthHandler() {
    this->_logger = SensorLogger::getInstance()->getLogger();
}

void AuthHandler::handle(ClientConnector *responder, const WsMessage& msg) {
    if (msg.error == 0) {
        SPDLOG_LOGGER_INFO(
                this->_logger,
                boost::str(boost::format("auth successfully with message: %s") % msg.toJson()));
        json msgJson = json::parse(msg.payload->toJson());
        responder->onIdChange(msgJson["id"]);
    }
    else {
        SPDLOG_LOGGER_ERROR(
                this->_logger,
                boost::str(boost::format("auth failed with message: %s") % msg.toJson()));
    }
}
