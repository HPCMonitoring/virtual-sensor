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

std::string Repository::getData(const Filter &filter)
{
    std::string result = "[";

    if (filter.datatype == PROCESS)
    {
        const std::string pidLiteral = filter.whereCondition->findPid();
        if (pidLiteral.length() > 0)
        {
            const pid_t pid = std::stoi(pidLiteral);
            Process proc(pid);
            result.append(proc.toJsonStr(filter.attrs));
        }
        else {
            std::vector<pid_t> allPids = this->getAllPids();
            const size_t numOfProcesses = allPids.size();
            for(size_t i = 0; i < numOfProcesses; ++i) {
                Process proc(allPids.at(i));
                result.append(proc.toJsonStr(filter.attrs));
                if(i != numOfProcesses - 1 )
                    result.push_back(',');
            }
        }
    }
    result.push_back(']');
    return result;
}
