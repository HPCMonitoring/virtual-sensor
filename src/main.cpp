#include "filter.h"
#include "repository.h"

void signalHandler(int signal)
{
    std::cout << "Terminating with exit code " << signal << " ...\n";
    exit(signal);
}

int main(int argc, char *argv[])
{
    // Register signal handler
    signal(SIGINT, signalHandler);

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
