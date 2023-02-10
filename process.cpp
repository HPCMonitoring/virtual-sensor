#include "process.h"
#ifdef __PROCESS_H__

ProcessInfo::ProcessInfo(pid_t pid)
{
    this->pid = pid;
    this->parentPid = NULL;
    this->uid = NULL;
    this->gid = NULL;
    this->executePath = "";
    this->command = "";
    this->memoryUsage = NULL;
    this->cpuTime = NULL;
    this->cpuUtilization = NULL;
    this->networkInBandwidth = NULL;
    this->networkOutBandwidth = NULL;
    this->ioRead = NULL;
    this->ioWrite = NULL;
}

pid_t ProcessInfo::getPid() {
    return this->pid;
}

#endif