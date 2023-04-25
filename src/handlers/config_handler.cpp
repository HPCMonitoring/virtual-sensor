#include "handlers/config_handler.h"
#include "worker_repository.h"
#include "sensor_logger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ConfigHandler::ConfigHandler() {
    this->_logger = SensorLogger::getInstance()->getLogger();
}

void ConfigHandler::handle(ClientConnector *responder, const WsMessage &msg)
{
    SPDLOG_LOGGER_INFO(this->_logger, "enter config handler");
    std::vector<json> topics = json::parse(msg.payload->toJson())["topics"].get<std::vector<json>>();
    // Clear all running workers
    // TODO: re-implement clearAll method to reuse producers
    SPDLOG_LOGGER_INFO(this->_logger, "clear all running workers");
    WorkerRepository::getInstance()->clearAll();

    std::vector<std::pair<std::string, KakfaClient::WorkerProp*>> jobs;

    SPDLOG_LOGGER_INFO(this->_logger, "start process topic");
    for (json topic : topics)
    {
        try
        {
            // TODO: Validate type
            // Create worker
            const std::string type = topic["type"];
            const std::string topicName = topic["topicName"];
            const std::string brokerUrl = topic["broker"];
            const int interval = topic["interval"].get<int>();
            const std::string prefixCommand = topic["prefixCommand"];
            std::vector<Attribute> projection;

            json fieldMap = topic["fields"];
            for (json::iterator it = fieldMap.begin(); it != fieldMap.end(); ++it)
            {
                projection.emplace_back(it.key(), it.value());
            }

            auto *filter = new Filter(PROCESS, projection, prefixCommand);
            auto *workerProp = new KakfaClient::WorkerProp(topicName, filter, interval, responder->getMetadata());
            std::pair<std::string, KakfaClient::WorkerProp*> pair (brokerUrl, workerProp);

            jobs.push_back(pair);
        }
        catch (std::exception &exception)
        {
            std::stringstream ss;
            ss << "error in process config job with message ";
            ss << exception.what();
            SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
            // Send response message
            WsMessage response = WsMessage::from(msg);
            response.error = WsErrorCode::UNKNOWN;
            response.msg = ss.str();

            responder->send(response);
            return;
        }
    }

    SPDLOG_LOGGER_INFO(this->_logger, "start add workers");
    for(auto j : jobs) {
        WorkerRepository::getInstance()->addWorker(j.first, j.second);
    }

    // Send response message
    WsMessage response = WsMessage::from(msg);
    response.error = WsErrorCode::SUCCESS;
    response.msg = "success";

    responder->send(response);
}
