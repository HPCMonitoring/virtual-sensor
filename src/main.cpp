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

    std::vector<Attribute> attrs;
    attrs.push_back(Attribute("pid", "processID"));
    attrs.push_back(Attribute("name", "pname"));
    attrs.push_back(Attribute("executePath", "execPath"));
    attrs.push_back(Attribute("command", "cmd"));

    // const Filter filter(PROCESS, attrs, "&& 2 == pid 1 || 2 == uid 10 < cpu 20");
    const Filter filter(PROCESS, attrs, "== pid 3727");
    Repository &r = Repository::getInstance();
    std::ofstream out("data.json");
    out << r.getData(filter);
    out.close();
    // while (true);
    return 0;
}
