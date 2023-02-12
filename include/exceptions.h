#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "main.h"

#define ERR_PROCESS_NOT_EXISTS "Process does not exists !"
#define ERR_INVALID_PID "Invalid process's ID !"

class ProcessNotExists : public std::exception
{
    virtual const char *what() const throw()
    {
        return ERR_PROCESS_NOT_EXISTS;
    }
};

class InvalidPID : public std::exception
{
    virtual const char *what() const throw()
    {
        return ERR_INVALID_PID;
    }
};

#endif