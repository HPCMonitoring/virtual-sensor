#include "schemas/memory.h"

Memory::Memory()
{
    std::ifstream infile("/proc/meminfo");
    std::string line;
    short numOfReadAttrs = 0;
    while (numOfReadAttrs < 8 && std::getline(infile, line))
    {
        numOfReadAttrs++;
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "MemTotal:")
            iss >> this->total;
        else if (token == "MemFree:")
            iss >> this->free;
        else if (token == "MemAvailable:")
            iss >> this->available;
        else if (token == "Buffers:")
            iss >> this->buffers;
        else if (token == "Cached:")
            iss >> this->cached;
        else if (token == "SwapTotal:")
            iss >> this->swapTotal;
        else if (token == "SwapFree:")
            iss >> this->swapFree;
        else if (token == "SwapCached:")
            iss >> this->swapCached;
        else
            numOfReadAttrs--;
    }
    infile.close();
}