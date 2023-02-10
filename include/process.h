#ifndef __PROCESS_H__
#define __PROCESS_H__
#include <string>

enum GetProcessFilter
{
    PID = 5,
    PARENT_PID = 6,
    UID = 8,
    GID = 9
};

class ProcessInfo
{
private:
    pid_t pid;
    pid_t parentPid;
    uid_t uid;
    gid_t gid;
    std::string executePath;
    std::string command;
    ulong memoryUsage; // In KB
    ulong cpuTime;
    float cpuUtilization;      // In %
    double networkInBandwidth; // What interface ???
    double networkOutBandwidth;
    ulong ioWrite; // In KB
    ulong ioRead;  // In KB
public:
    ProcessInfo(pid_t);
    pid_t getPid();
};

#endif