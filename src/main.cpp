#include "recorder.h"
#include "ws_manager_client.h"
#include "handlers/auth_handler.h"
#include "handlers/sys_info_handler.h"
#include "handlers/config_handler.h"

std::atomic<bool> terminateFlag(false);
std::thread mainThread;

void setupWsClient() {
    SensorManagerClient *client = SensorManagerClient::GetInstance();
    auto *authHandler = new AuthHandler();
    auto *sysInfoHandler = new SysInfoHandler();
    auto *configHandler = new ConfigHandler();

    client->registerHandler(WsCommand::AUTH, authHandler);
    client->registerHandler(WsCommand::SYS_INFO, sysInfoHandler);
    client->registerHandler(WsCommand::CONFIG, configHandler);

    client->setupAndStart();
}

void mainThreadHandler()
{
    Recorder recorder("1915940", "localhost:9092");

    std::vector<Attribute> projection;
    projection.push_back(Attribute(N_PID, "processID"));
    projection.push_back(Attribute(N_NAME, "pname"));
    projection.push_back(Attribute(N_EXEC_PATH, "execPath"));
    projection.push_back(Attribute(N_CMD, "cmd"));
    projection.push_back(Attribute(N_CPUU));
    projection.push_back(Attribute(N_CPUT));
    projection.push_back(Attribute(N_VMU, "VmUsage"));
    projection.push_back(Attribute(N_PMU, "PmUsage"));
    Filter filter(PROCESS, projection, "&& 2 == pid 1 < virtualMemoryUsage 200000");

    Recorder::WorkerProp *prop = new Recorder::WorkerProp("hello-world", &filter, 5);

    recorder.addWorker(prop);

    while (false == terminateFlag.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Cleaning resources before exitting ...\n";
}

void signalHandler(int signal)
{
    std::cout << "\nInterrupt signal (" << signal << ") received.\n";
    terminateFlag.store(true, std::memory_order_release);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
    mainThread = std::thread(mainThreadHandler);
    mainThread.join();
    return 0;
}
