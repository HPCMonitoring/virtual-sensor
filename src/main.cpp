#include "ws_manager_client.h"
#include "handlers/auth_handler.h"
#include "handlers/sys_info_handler.h"
#include "handlers/config_handler.h"
#include "repository/repository.h"
#include "repository/filter.h"
#include <iostream>

std::thread mainThread;
SensorManagerClient *client;

void mainThreadHandler()
{
    client = SensorManagerClient::getInstance();
    auto *authHandler = new AuthHandler();
    auto *sysInfoHandler = new SysInfoHandler();
    auto *configHandler = new ConfigHandler();

    client->registerHandler(WsCommand::AUTH, authHandler);
    client->registerHandler(WsCommand::SYS_INFO, sysInfoHandler);
    client->registerHandler(WsCommand::CONFIG, configHandler);

    client->setupAndStart();
}

void signalHandler(int signal)
{
    std::cout << "\nInterrupt signal (" << signal << ") received.\n";
    delete client;
}

void readSampleProc();
void readSampleCpu();
void readSampleNetwork();
void readSampleMemory();
void readSampleIO();
void readSampleDisk();

int main(int argc, char *argv[])
{
    // signal(SIGINT, signalHandler);
    // mainThread = std::thread(mainThreadHandler);
    // mainThread.join();

    readSampleProc();
    // readSampleCpu();
    // readSampleMemory();
    // readSampleNetwork();
    // readSampleIO();
    // readSampleDisk();

    return 0;
}



void readSampleProc()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("name"));
    attrs.push_back(Attribute("pid", "processID"));
    attrs.push_back(Attribute("parentPid"));
    attrs.push_back(Attribute("uid"));
    attrs.push_back(Attribute("gid"));
    // attrs.push_back(Attribute("executePath"));
    // attrs.push_back(Attribute("command"));
    attrs.push_back(Attribute("virtualMemory"));
    attrs.push_back(Attribute("physicalMemory"));
    attrs.push_back(Attribute("cpuTime"));
    attrs.push_back(Attribute("cpuUsage"));
    attrs.push_back(Attribute("networkIn"));
    attrs.push_back(Attribute("networkOut"));
    attrs.push_back(Attribute("readKBs"));
    attrs.push_back(Attribute("writeKBs"));

    Filter filter(PROCESS, attrs, "%= executePath discord");
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << '[';
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << ']';
}

void readSampleCpu()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("order"));
    attrs.push_back(Attribute("user"));
    attrs.push_back(Attribute("nice"));
    attrs.push_back(Attribute("system"));
    attrs.push_back(Attribute("idle"));
    attrs.push_back(Attribute("iowait"));
    attrs.push_back(Attribute("irq"));
    attrs.push_back(Attribute("softirq"));
    attrs.push_back(Attribute("steal"));
    attrs.push_back(Attribute("guest"));
    attrs.push_back(Attribute("guestNice"));

    Filter filter(CPU, attrs);
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << "[\n";
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << '\t';
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void readSampleMemory()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("total"));
    attrs.push_back(Attribute("free"));
    attrs.push_back(Attribute("available"));
    attrs.push_back(Attribute("buffers"));
    attrs.push_back(Attribute("cached"));
    attrs.push_back(Attribute("swapTotal"));
    attrs.push_back(Attribute("swapFree"));
    attrs.push_back(Attribute("swapCached"));

    Filter filter(MEMORY, attrs);
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << "[\n";
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << '\t';
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void readSampleNetwork()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("name"));
    attrs.push_back(Attribute("receive"));
    attrs.push_back(Attribute("transmit"));

    Filter filter(NETWORK_INTERFACE, attrs);
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << "[\n";
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << '\t';
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void readSampleIO()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("device"));
    attrs.push_back(Attribute("tps"));
    attrs.push_back(Attribute("readPerSec"));
    attrs.push_back(Attribute("read"));
    attrs.push_back(Attribute("writePerSec"));
    attrs.push_back(Attribute("write"));
    attrs.push_back(Attribute("discardPerSec"));
    attrs.push_back(Attribute("discard"));

    Filter filter(IO, attrs);
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << "[\n";
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << '\t';
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}

void readSampleDisk()
{
    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("filesystem"));
    attrs.push_back(Attribute("size"));
    attrs.push_back(Attribute("used"));
    attrs.push_back(Attribute("available"));
    attrs.push_back(Attribute("usedPercentage"));
    attrs.push_back(Attribute("mountedOn"));

    Filter filter(DISK, attrs);
    Repository &r = Repository::getInstance();

    auto data = r.getData(&filter);
    std::cout << "[\n";
    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << '\t';
        std::cout << data.at(i) << '\n';
        if (i != data.size() - 1)
            std::cout << ',';
    }
    std::cout << "]\n";
}
