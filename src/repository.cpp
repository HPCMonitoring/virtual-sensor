#include "repository.h"

std::string Repository::getData(const Filter& filter) {
    std::string result = "[";

    if(filter.datatype == PROCESS) {
        const std::string pidLiteral = filter.whereCondition->findPid();
        if(pidLiteral.length() > 0) {
            const pid_t pid = std::stoi(pidLiteral);
            Process proc(pid);
            result.append(proc.toJsonStr(filter.attrs));
        }
    }
    result.push_back(']');
    return result;
}