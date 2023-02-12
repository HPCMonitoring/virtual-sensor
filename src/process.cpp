#include "process.h"

#ifdef __PROCESS_H__

inline bool fileExists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

ProcessInfo::ProcessInfo(pid_t pid)
{
    if (pid < 0)
        throw ERR_INVALID_PID;

    std::string processEntryDir = "/proc/" + std::to_string(pid);
    this->statusFilename = processEntryDir + "/status";
    if (!fileExists(this->statusFilename))
        throw ERR_PROCESS_NOT_EXISTS;

    this->executePath = processEntryDir + "/exe";
    this->commandFilename = processEntryDir + "/cmdline";

    this->pid = pid;
    this->parentPid = -1;
    this->uid = this->gid = UINT_MAX;
    this->memoryUsage = this->cpuTime = ULONG_MAX;
    this->cpuUtilization = -1;
    this->networkInBandwidth = this->networkOutBandwidth = -1;
    this->ioRead = this->ioWrite = ULONG_MAX;
    this->name = this->executePath = this->command = "";
}

std::string ProcessInfo::getName()
{
    if (this->name.length() != 0)
        return this->name;
    this->_readProcessInfoFile(ProcessStatusInfoLine::NAME);
    return this->name;
}

pid_t ProcessInfo::getPid()
{
    return this->pid;
}

pid_t ProcessInfo::getParentPid()
{
    if (this->parentPid >= 0)
        return this->parentPid;
    this->_readProcessInfoFile(ProcessStatusInfoLine::PARENT_PID);
    return this->parentPid;
}

uid_t ProcessInfo::getUid()
{
    if (this->uid != UINT_MAX)
        return this->uid;
    this->_readProcessInfoFile(ProcessStatusInfoLine::UID);
    return this->uid;
}

gid_t ProcessInfo::getGid()
{
    if (this->gid != UINT_MAX)
        return this->gid;
    this->_readProcessInfoFile(ProcessStatusInfoLine::GID);
    return this->gid;
}

std::string ProcessInfo::getExecutePath()
{
    if (this->executePath.length() > 0)
        return this->executePath;
    return "";
}

std::string ProcessInfo::getCommand()
{
    if (this->command.length() > 0)
        return this->command;
    return "";
}

ulong ProcessInfo::getMemoryUsage()
{
    if (this->memoryUsage != ULONG_MAX)
        return this->memoryUsage;
    return 0;
}

ulong ProcessInfo::getCpuTime()
{
    if (this->cpuTime != ULONG_MAX)
        return this->cpuTime;
    return 0;
}

float ProcessInfo::getCpuUtilization()
{
    if (this->cpuUtilization >= 0)
        return this->cpuUtilization;
    return 0;
}

double ProcessInfo::getNetworkInBandwidth()
{
    if (this->networkInBandwidth >= 0)
        return this->networkInBandwidth;
    return 0;
}

double ProcessInfo::getNetworkOutBandwidth()
{
    if (this->networkOutBandwidth >= 0)
        return this->networkOutBandwidth;
    return 0;
}

ulong ProcessInfo::getIoRead()
{
    if (this->ioRead != ULONG_MAX)
        return this->ioRead;
    return 0;
}

ulong ProcessInfo::getIoWrite()
{
    if (this->ioWrite != ULONG_MAX)
        return this->ioWrite;
    return 0;
}

void ProcessInfo::print() {
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Pid: " << this->pid << std::endl;
    std::cout << "Parent Pid: " << this->parentPid << std::endl;
    std::cout << "Uid: " << this->uid << std::endl;
    std::cout << "Gid: " << this->gid << std::endl;
}

#endif