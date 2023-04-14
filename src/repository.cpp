#include "repository.h"

bool isProcessDirectory(const std::string &name)
{
    // Check if the directory name consists entirely of digits
    const size_t length = name.size();
    for (size_t i = 0; i < length; ++i)
    {
        if (name.at(i) < '0' || name.at(i) > '9')
        {
            return false;
        }
    }
    return true;
}

Repository &Repository::getInstance()
{
    static Repository instance;
    return instance;
}

std::vector<std::string> Repository::getData(const Filter *filter)
{
    std::vector<std::string> results;
    // std::string result = "[";

    if (filter->datatype == PROCESS)
    {
        const std::string pidLiteral = filter->selection != nullptr ? filter->selection->findPid() : "";
        if (pidLiteral.length() > 0)
        {
            const pid_t pid = std::stoi(pidLiteral);
            Process proc(pid);
            if (proc.exists() && filter->selection->validate(&proc))
                results.push_back(filter->iterate(&proc));
        }
        else
        {
            std::vector<pid_t> allPids = this->getAllPids();
            const size_t numOfProcesses = allPids.size();
            for (size_t i = 0; i < numOfProcesses; ++i)
            {
                Process proc(allPids.at(i));
                if (!proc.exists() || !filter->selection->validate(&proc))
                    continue;

                results.push_back(filter->iterate(&proc));
            }
        }
    }
    else if (filter->datatype == NETWORK_INTERFACE)
    {
    }
    else if (filter->datatype == MEMORY)
    {
    }
    else if (filter->datatype == CPU)
    {
    }
    else if (filter->datatype == IO)
    {
    }
    else if (filter->datatype == DISK)
    {
    }

    return results;
}
