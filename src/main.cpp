#include "filter.h"
#include "process.h"

void signalHandler(int signal)
{
    std::cout << "Terminating with exit code " << signal << " ...\n";
    exit(signal);
}

int main(int argc, char *argv[])
{
    // Register signal handler
    signal(SIGINT, signalHandler);

    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("pid", "processID"));
    attrs.push_back(Attribute("name", "pname"));
    attrs.push_back(Attribute("executePath", "execPath"));
    attrs.push_back(Attribute("command", "cmd"));

    Process p(1);

    std::cout << p.toJsonStr(attrs) << std::endl;

    // while (true);
    return 0;
}
