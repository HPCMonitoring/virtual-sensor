#include "ws_manager_client.h"
#include "handlers/auth_handler.h"
#include "handlers/sys_info_handler.h"
#include "filter.h"
#include "repository.h"

void setupWsClient() {
    SensorManagerClient *client = SensorManagerClient::GetInstance();
    auto *authHandler = new AuthHandler();
    auto *sysInfoHandler = new SysInfoHandler();

    client->registerHandler(WsCommand::AUTH, authHandler);
    client->registerHandler(WsCommand::SYS_INFO, sysInfoHandler);

    client->setupAndStart();
}

void signalHandler(int signal)
{
    std::cout << "Terminating with exit code " << signal << " ...\n";
    exit(signal);
}

int main(int argc, char *argv[])
{

    // Uncomment this line to setup ws client
    //    setupWsClient();


    std::vector<Attribute> projection;
    projection.push_back(Attribute("pid", "processID"));
    projection.push_back(Attribute("name", "pname"));
    projection.push_back(Attribute("executePath", "execPath"));
    projection.push_back(Attribute("command", "cmd"));

    const Filter filter(PROCESS, projection, "%= name syste[a-z]+");
    Repository &r = Repository::getInstance();
    std::cout << r.getData(filter) << std::endl;
    // while (true);
    return 0;
}
