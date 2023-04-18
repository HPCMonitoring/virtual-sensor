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
    attrs.push_back(Attribute("name"));
    attrs.push_back(Attribute("pid", "processID"));
    attrs.push_back(Attribute("parentPid"));
    attrs.push_back(Attribute("uid"));
    attrs.push_back(Attribute("gid"));
    attrs.push_back(Attribute("executePath"));
    attrs.push_back(Attribute("command"));
    attrs.push_back(Attribute("virtualMemory"));
    attrs.push_back(Attribute("physicalMemory"));
    attrs.push_back(Attribute("cpuTime"));
    attrs.push_back(Attribute("cpuUsage"));
    attrs.push_back(Attribute("networkInBandwidth"));
    attrs.push_back(Attribute("networkOutBandwidth"));
    attrs.push_back(Attribute("readKBs"));
    attrs.push_back(Attribute("writeKBs"));

    Filter filter(PROCESS, attrs, "%= executePath .*slack.*");
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << '[';
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << data.at(i) << '\n';
        if(i != data.size() - 1) std::cout << ',';
    }
    std::cout << ']';

    return 0;
}
