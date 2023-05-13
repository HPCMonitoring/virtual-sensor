#include <fstream>
#include <ws_manager_client.h>
#include "handlers/plain_json_str.h"

SensorManagerClient *SensorManagerClient::_pinstance{nullptr};
std::mutex SensorManagerClient::_mutex;
std::string SensorManagerClient::CONF_F_NAME = "wsconfig.json";

SensorManagerClient::SensorManagerClient()
{
    this->_logger = SensorLogger::getInstance()->getLogger();
    // TODO: must replace periodically retry by fibonacci retry approach
    this->_nextConnRetry = 10;
    this->_stopFlag = false;
    std::ifstream f(CONF_F_NAME);
    if (f.is_open())

    {
        SPDLOG_LOGGER_INFO(this->_logger, "Enter reading config file");
        this->_config = json::parse(f);
        f.close();
    }
}

SensorManagerClient *SensorManagerClient::getInstance()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_pinstance == nullptr)
    {
        _pinstance = new SensorManagerClient();
    }
    return _pinstance;
}

std::string SensorManagerClient::buildConnStr()
{
    std::string conn = "ws://";
    conn.append(this->_config["server"]).append("/ws").append("?");

    std::string clusterParam = std::regex_replace(std::string(this->_config["cluster"]), std::regex(" "), URL_SPACE);
    std::string nameParam = std::regex_replace(std::string(this->_config["name"]), std::regex(" "), URL_SPACE);
    conn.append("&cluster=").append(clusterParam);
    conn.append("&name=").append(nameParam);

    if (this->_config.contains("id"))
    {
        std::string idParam = std::regex_replace(std::string(this->_config["id"]), std::regex(" "), URL_SPACE);
        conn.append("&id=").append(idParam);
    }
    return conn;
}

void SensorManagerClient::registerHandler(WsCommand cmd, CmdHandler *handler)
{
    _fmap[cmd] = handler;
}

void SensorManagerClient::onMessage(const ix::WebSocketMessagePtr &msg)
{
    std::stringstream ss;
    ss << "onMessage called with message: " << msg->str;
    SPDLOG_LOGGER_INFO(this->_logger, ss.str());

    json msgJson = json::parse(msg->str);
    try
    {
        const auto cmd = msgJson["cmd"].get<WsCommand>();
        if (_fmap.count(cmd) > 0)
        {
            WsMessage message;
            message.cmd = cmd;
            message.msg = msgJson.contains("message") ? msgJson["message"].get<std::string>() : "";
            message.error = msgJson.contains("error") ? msgJson["error"].get<int>() : 0;
            message.coordId = msgJson.contains("coordId") ? msgJson["coordId"].get<std::string>() : "";
            auto *plainJsonStr = new PlainJsonStr(msgJson["payload"].dump());
            message.payload = plainJsonStr;
            _fmap.at(cmd)->handle(this, message);
        }
        else
        {
            std::stringstream ss;
            ss << "no handler for command: " << cmd;
            SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
        }

    }
    catch (const std::exception &e)
    {
        std::stringstream ss;
        ss << "onMessage handling error with message: " << e.what();
        SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
    }
}

void SensorManagerClient::onOpen(const ix::WebSocketMessagePtr &msg)
{
    std::stringstream ss;
    ss << "onOpen called with message: " << msg->str;
    SPDLOG_LOGGER_INFO(this->_logger, ss.str());
}

void SensorManagerClient::onError(const ix::WebSocketMessagePtr &msg)
{
    std::stringstream ss;
    ss << "onError called with message: " << msg->errorInfo.reason;
    SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
}

void SensorManagerClient::onClose(const ix::WebSocketMessagePtr &msg)
{
    std::stringstream ss;
    ss << "onClose called with code: " << msg->closeInfo.code
        << ", message: " << msg->closeInfo.reason;
    SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
}

void SensorManagerClient::setupAndStart()
{
    while (!this->_stopFlag)
    {
        std::thread thread_obj(&SensorManagerClient::run, this);
        thread_obj.join();
        std::stringstream ss;
        ss << "websocket client run done and start re-run in next " << this->_nextConnRetry << " sec";
        SPDLOG_LOGGER_INFO(this->_logger, ss.str());
        std::this_thread::sleep_for (std::chrono::seconds(this->_nextConnRetry));
    }
}

void SensorManagerClient::send(const WsMessage &res)
{
    _webSocket.send(res.toJson());
}

void SensorManagerClient::onIdChange(std::string id)
{
    this->_config["id"] = id;
    std::ofstream outfile(CONF_F_NAME, std::ios::trunc);
    outfile << this->_config.dump();
    outfile.close();
}

SensorManagerClient::~SensorManagerClient()
{
    for (auto kv : this->_fmap)
    {
        delete kv.second;
    }
}

void SensorManagerClient::run()
{
    std::string uri = this->buildConnStr();
    _webSocket.setUrl(uri);
    ix::WebSocketHttpHeaders headers;
    headers["authorization"] = this->_config["auth_token"];
    _webSocket.setExtraHeaders(headers);
    _webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg)
                                    {
           switch (msg->type) {
               case ix::WebSocketMessageType::Message:
                   this->onMessage(msg);
                   break;
               case ix::WebSocketMessageType::Open:
                   onOpen(msg);
                   break;
               case ix::WebSocketMessageType::Close:
                   onClose(msg);
                   break;
               case ix::WebSocketMessageType::Error:
                   onError(msg);
                   break;
               default:
                   break;
           } });
    _webSocket.disableAutomaticReconnection();
    _webSocket.setPingInterval(5);
    try
    {
        _webSocket.run();
        _webSocket.close();
    }
    catch (std::exception &e)
    {
        SPDLOG_LOGGER_INFO(this->_logger, std::string("Error in run websocket client with message: ").append(e.what()));
    }
}

std::unordered_map<std::string, std::string> SensorManagerClient::getMetadata() {
    try
    {
        const std::string PREFIX = "ss.";
        std::unordered_map<std::string, std::string> metadata;
        metadata.insert({PREFIX + "cluster", this->_config.value("cluster", "")});
        metadata.insert({PREFIX + "name", this->_config.value("name", "")});
        metadata.insert({PREFIX + "id", this->_config.value("id", "")});

        return metadata;
    }
    catch (const std::exception &e)
    {
        std::stringstream ss;
        ss << "getMetadata error with message: " << e.what();
        SPDLOG_LOGGER_ERROR(this->_logger, ss.str());
    }


    std::unordered_map<std::string, std::string> empty;
    return empty;
}
