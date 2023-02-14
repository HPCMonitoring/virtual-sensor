#ifndef __PROCESS_REPOSITORY_H__
#define __PROCESS_REPOSITORY_H__

#include "process.h"

class Sensor
{
private:
    // Hashmap
    std::unordered_map<std::string, std::string> clients;
public:
    std::vector<std::string> getProcessInfo(const std::vector<std::string> &);
};

#endif