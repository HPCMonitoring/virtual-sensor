#include "schemas/io.h"

IOStat::IOStat(const std::string &dataline)
{
    std::istringstream iss(dataline);
    iss >> this->device;
    iss >> this->tps;
    iss >> this->readPerSec;
    iss >> this->writePerSec;
    iss >> this->discardPerSec;
    iss >> this->read;
    iss >> this->write;
    iss >> this->discard;

    std::replace(this->readPerSec.begin(), this->readPerSec.end(), ',', '.');
    std::replace(this->writePerSec.begin(), this->writePerSec.end(), ',', '.');
    std::replace(this->discardPerSec.begin(), this->discardPerSec.end(), ',', '.');
    std::replace(this->tps.begin(), this->tps.end(), ',', '.');
}