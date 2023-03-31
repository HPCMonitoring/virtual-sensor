#ifndef __PROCESS_REPOSITORY_H__
#define __PROCESS_REPOSITORY_H__

#include "main.h"
#include "filter.h"
#include "process.h"

class Repository
{
private:
    Repository(){};
    ~Repository(){};
    // Mark as ProcessRepository does not have copy constructor & and cannot assign to another variables
    Repository(const Repository &) = delete;
    Repository &operator=(const Repository &) = delete;

public:
    static Repository &getInstance()
    {
        static Repository instance;
        return instance;
    }
    std::string getData(const Filter&);
};

#endif