#include "handlers/config_handler.h"
#include "worker_repository.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void ConfigHandler::handle(ClientConnector *responder, const WsMessage &msg)
{
    std::vector<json> topics = json::parse(msg.payload->toJson())["topics"].get<std::vector<json>>();
    // Clear all running workers
    // TODO: re-implement clearAll method to reuse producers
    WorkerRepository::getInstance()->clearAll();

    for (json topic : topics)
    {
        try
        {
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
            auto *workerProp = new KakfaClient::WorkerProp(topicName, filter, interval);
            WorkerRepository::getInstance()->addWorker(brokerUrl, workerProp);

            // Send response message
            WsMessage response = WsMessage::from(msg);
            response.error = WsErrorCode::SUCCESS;
            response.msg = "success";

            responder->send(response);
        }
        catch (std::exception &exception)
        {
            std::cout << exception.what();
            // TODO: handle parse fail exception
        }
    }
}
