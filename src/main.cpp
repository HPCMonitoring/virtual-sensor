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
    projection.push_back(Attribute("cpuUsage", ""));
    projection.push_back(Attribute("cpuTime", ""));
    projection.push_back(Attribute(N_VMU, "VmUsage"));
    projection.push_back(Attribute(N_PMU, "PmUsage"));

    const Filter filter(PROCESS, projection, "&& 2 < virtualMemoryUsage 20000 >= virtualMemoryUsage 17500");
    // const Filter filter(PROCESS, projection, "AND 1 == pid 1");
    Repository &r = Repository::getInstance();
    std::cout << r.getData(filter) << std::endl;
    
    // std::cout << std::stod("-1") << std::endl;
    // while (true);
    return 0;
}
