#include "schemas/disk_usage.h"

DiskUsage::DiskUsage(const std::string &dataline)
{
    std::istringstream iss(dataline);
    iss >> this->filesystem >> this->size >> this->used >> this->available >> this->usedPercentage >> this->mountedOn;
}