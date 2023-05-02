#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#define ERR_PROCESS_NOT_EXISTS "Process does not exists !"
#define ERR_INVALID_PID "Invalid process's ID !"
#define ERR_UNIMPLEMENTED "Unimplemented !"
#define ERR_CREATE_PRODUCER "Failed to create kafka producer"

class ProcessNotExists : public std::exception
{
    virtual const char *what() const throw()
    {
        return ERR_PROCESS_NOT_EXISTS;
    }
};

class Unimplemented : public std::exception
{
    virtual const char *what() const throw()
    {
        return ERR_UNIMPLEMENTED;
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