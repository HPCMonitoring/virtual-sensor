#ifndef __DISK_USAGE_H__
#define __DISK_USAGE_H__

#include <string>
#include <sstream>

class DiskUsage
{
public:
    std::string filesystem;
    std::string size;
    std::string used;
    std::string available;
    std::string usedPercentage;
    std::string mountedOn;

public:
    DiskUsage(const std::string &);
};

#endif