#include <fstream>
#include <boost/format.hpp>
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

    std::string spaceReplacement = "%20";
    conn.append(this->_config["server"]).append("/ws").append("?");
    conn.append("&cluster=").append(utils::strReplace(this->_config["cluster"], " ", spaceReplacement));
    conn.append("&name=").append(utils::strReplace(this->_config["name"], " ", spaceReplacement));
    if (this->_config.contains("id")) {
        conn.append("&id=").append(utils::strReplace(this->_config["id"], " ", spaceReplacement));
    }
    return conn;
}

void SensorManagerClient::registerHandler(WsCommand cmd, CmdHandler *handler)
{
    _fmap[cmd] = handler;
}

void SensorManagerClient::on_message(const ix::WebSocketMessagePtr &msg)
{
    SPDLOG_LOGGER_INFO(
        this->_logger,
        boost::str(boost::format("on_message called with message: %s") % msg->str));

    json msgJson = json::parse(msg->str);
    try
    {
        const auto cmd = msgJson["cmd"].get<WsCommand>();
        if (_fmap.count(cmd) > 0) {
            WsMessage message;
            message.cmd = cmd;
            message.msg = msgJson.contains("message") ? msgJson["message"].get<std::string>() : "";
            message.error = msgJson.contains("error") ? msgJson["error"].get<int>() : 0;
            message.coordId = msgJson.contains("coordId") ? msgJson["coordId"].get<std::string>() : "";
            auto *plainJsonStr = new PlainJsonStr( msgJson["payload"].dump());
            message.payload = plainJsonStr;
            _fmap.at(cmd)->handle(this, message);
        }
        else
        {
            SPDLOG_LOGGER_ERROR(
                this->_logger,
                boost::str(boost::format("no handler for command: %s") % cmd));
        }

    }
    catch (const std::exception &e)
    {
        SPDLOG_LOGGER_ERROR(
            this->_logger,
            boost::str(boost::format("on_open called with message: %s") % e.what()));
    }
}

void SensorManagerClient::on_open(const ix::WebSocketMessagePtr &msg)
{
    SPDLOG_LOGGER_INFO(
        this->_logger,
        boost::str(boost::format("on_open called with message: %s") % msg->str));
}

void SensorManagerClient::on_error(const ix::WebSocketMessagePtr &msg)
{
    SPDLOG_LOGGER_ERROR(
        this->_logger,
        boost::str(boost::format("on_error called with message: %s") % msg->errorInfo.reason));
}

void SensorManagerClient::on_close(const ix::WebSocketMessagePtr &msg)
{
    SPDLOG_LOGGER_ERROR(
        this->_logger,
        boost::str(boost::format("on_close called with code: %s,  message: %s") % msg->closeInfo.code % msg->closeInfo.reason));
}

void SensorManagerClient::setupAndStart()
{
    while(!this->_stopFlag) {
        std::thread thread_obj(&SensorManagerClient::run, this);
        thread_obj.join();
        SPDLOG_LOGGER_INFO(
            this->_logger,
            boost::str(boost::format("websocket client run done and start re-run in next %d sec") % this->_nextConnRetry));
        std::this_thread::sleep_for (std::chrono::seconds(this->_nextConnRetry));
    }
}

void SensorManagerClient::send(const WsMessage &res)
{
    _webSocket.send(res.toJson());
}

void SensorManagerClient::onIdChange(std::string id) {
    this->_config["id"] = id;
    std::ofstream outfile (CONF_F_NAME, std::ios::trunc);
    outfile << this->_config.dump();
    outfile.close();
}

SensorManagerClient::~SensorManagerClient()
{
    for(auto kv : this->_fmap) {
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
                   this->on_message(msg);
                   break;
               case ix::WebSocketMessageType::Open:
                   on_open(msg);
                   break;
               case ix::WebSocketMessageType::Close:
                   on_close(msg);
                   break;
               case ix::WebSocketMessageType::Error:
                   on_error(msg);
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
