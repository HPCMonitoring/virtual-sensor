#include "filter.h"
#include "recorder.h"

void signalHandler(int signal)
{
    std::cout << "Terminating with exit code " << signal << " ...\n";
    exit(signal);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
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

    recorder.addWorker("hello-world", &filter, 5);

    while (true);
    return 0;
}
