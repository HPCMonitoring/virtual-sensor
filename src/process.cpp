#include "process.h"
#ifdef __PROCESS_H__

ProcessInfo::ProcessInfo(pid_t pid)
{
    this->pid = pid;
}

pid_t ProcessInfo::getPid() {
    return this->pid;
}

#endif