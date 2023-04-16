#include "schemas/cpu.h"

Cpu::Cpu(const std::string &dataline)
{
    std::istringstream iss(dataline);
    iss >> this->order >> this->user >> this->nice >> this->system >> this->idle >> this->iowait >> this->irq >> this->softirq >> this->steal >> this->guest >> this->guestNice;
}