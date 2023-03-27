#include "ws_manager_client.h"
#include "handlers/auth_handler.h"
#include "handlers/sys_info_handler.h"
#include "sensor_logger.h"
#include "iostream"
#include "process.h"

void setupWsClient() {
    SensorManagerClient *client = SensorManagerClient::GetInstance();
    auto *authHandler = new AuthHandler();
    auto *sysInfoHandler = new SysInfoHandler();

    client->registerHandler(WsCommand::AUTH, authHandler);
    client->registerHandler(WsCommand::SYS_INFO, sysInfoHandler);

    client->setupAndStart();
}
int main(int argc, char *argv[])
{
//    ProcessInfo p(1);
//    printProcessInfo(&p);
// Uncomment this line to setup ws client
    setupWsClient();


    return 0;
}