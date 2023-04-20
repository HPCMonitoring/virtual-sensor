#ifndef __SENSOR_MANAGER_WEBSOCKET_CLIENT_H__
#define __SENSOR_MANAGER_WEBSOCKET_CLIENT_H__

#include "sensor_logger.h"
#include "client_connector.h"
#include "handlers/cmd_handler.h"

#define URL_SPACE "%20"

using json = nlohmann::json;

class SensorManagerClient : public ClientConnector
{
private:
    static std::string CONF_F_NAME;
    static SensorManagerClient *_pinstance;
    static std::mutex _mutex;
    std::unordered_map<int, CmdHandler *> _fmap;
    std::shared_ptr<spdlog::logger> _logger;
    json _config;
    std::atomic<bool> _stopFlag;
    int _nextConnRetry;
    ix::WebSocket _webSocket;
    void run();

protected:
    SensorManagerClient();
    std::string buildConnStr();
    void on_message(const ix::WebSocketMessagePtr &msg);
    void on_open(const ix::WebSocketMessagePtr &msg);
    void on_error(const ix::WebSocketMessagePtr &msg);
    void on_close(const ix::WebSocketMessagePtr &msg);

public:
    static SensorManagerClient *getInstance();
    ~SensorManagerClient();
    SensorManagerClient(SensorManagerClient &other) = delete;
    void operator=(const SensorManagerClient &) = delete;
    void setupAndStart();
    void registerHandler(WsCommand cmd, CmdHandler *handler);
    void send(const WsMessage &res) override;
    void onIdChange(std::string id) override;
};

#endif
