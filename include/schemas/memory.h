#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "main.h"

class Memory
{
public:
    std::string total;
    std::string free;
    std::string available;
    std::string buffers;
    std::string cached;
    std::string swapTotal;
    std::string swapFree;
    std::string swapCached;

    Memory();
};

#endif