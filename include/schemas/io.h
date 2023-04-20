#ifndef __IO_H__
#define __IO_H__

#include "main.h"

class IOStat {
public:
    std::string device;
    std::string tps;
    std::string readPerSec;
    std::string read;
    std::string writePerSec;
    std::string write;
    std::string discardPerSec;
    std::string discard;
public:
    IOStat(const std::string&);
};


#endif