#ifndef __PROCESS_REPOSITORY_H__
#define __PROCESS_REPOSITORY_H__

#include "main.h"

class ProcessRepository
{
private:
    ProcessRepository()
    {
        std::cout << "Constructor ProcessRepository" << std::endl;
    }

    ~ProcessRepository()
    {
        std::cout << "Destructor ProcessRepository" << std::endl;
    }

    ProcessRepository(const ProcessRepository &) = delete;
    ProcessRepository &operator=(const ProcessRepository &) = delete;

public:
    static ProcessRepository &getInstance()
    {
        static ProcessRepository instance;
        return instance;
    }

    unsigned long long getAddress()
    {
        return reinterpret_cast<unsigned long long>(this);
    }
};

#endif