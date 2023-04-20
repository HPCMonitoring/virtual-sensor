#ifndef __CPU_H__
#define __CPU_H__

#include "main.h"

class Cpu
{
public:
    std::string order;
    std::string user;
    std::string nice;
    std::string system;
    std::string idle;
    std::string iowait;
    std::string irq;
    std::string softirq;
    std::string steal;
    std::string guest;
    std::string guestNice;

public:
    Cpu(const std::string &);
};

#endif