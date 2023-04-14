// #include "recorder.h"
// #include "ws_manager_client.h"
// #include "handlers/auth_handler.h"
// #include "handlers/sys_info_handler.h"
// #include "handlers/config_handler.h"

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

    return 0;
}
