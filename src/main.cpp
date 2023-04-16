// #include "recorder.h"
// #include "ws_manager_client.h"
// #include "handlers/auth_handler.h"
// #include "handlers/sys_info_handler.h"
// #include "handlers/config_handler.h"

#include "repository/repository.h"
#include "repository/filter.h"

// std::thread mainThread;
// SensorManagerClient *client;

// void mainThreadHandler()
// {
//     client = SensorManagerClient::getInstance();
//     auto *authHandler = new AuthHandler();
//     auto *sysInfoHandler = new SysInfoHandler();
//     auto *configHandler = new ConfigHandler();

//     client->registerHandler(WsCommand::AUTH, authHandler);
//     client->registerHandler(WsCommand::SYS_INFO, sysInfoHandler);
//     client->registerHandler(WsCommand::CONFIG, configHandler);

//     client->setupAndStart();
// }

// void signalHandler(int signal)
// {
//     std::cout << "\nInterrupt signal (" << signal << ") received.\n";
// }

int main(int argc, char *argv[])
{
    // signal(SIGINT, signalHandler);
    // mainThread = std::thread(mainThreadHandler);
    // mainThread.join();
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("order"));
    attrs.push_back(Attribute("user"));
    attrs.push_back(Attribute("nice"));
    attrs.push_back(Attribute("system"));
    attrs.push_back(Attribute("idle"));
    attrs.push_back(Attribute("iowait"));
    attrs.push_back(Attribute("irq"));
    attrs.push_back(Attribute("steal"));

    Filter filter(CPU, attrs);
    Repository &r = Repository::getInstance();
    auto data = r.getData(&filter);
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << data.at(i) << std::endl;
    }

    return 0;
}
