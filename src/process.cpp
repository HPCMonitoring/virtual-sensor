#include "process.h"
#include "exceptions.h"

#ifdef __PROCESS_H__

inline bool fileExists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

ProcessInfo::ProcessInfo(pid_t pid)
{
    if (pid < 0)
        throw InvalidPID();

    std::string processEntryDirname = "/proc/" + std::to_string(pid);
    this->statusFilename = processEntryDirname + "/status";
    if (!fileExists(this->statusFilename))
        throw ProcessNotExists();

    this->processEntryDirname = processEntryDirname;

    this->pid = pid;
    this->parentPid = -1;
    this->uid = this->gid = UINT_MAX;
    this->virtualMemoryUsage = this->physicalMemoryUsage = this->cpuTime = ULONG_MAX;
    this->cpuUtilization = -1;
    this->networkInBandwidth = this->networkOutBandwidth = -1;
    this->ioRead = this->ioWrite = ULONG_MAX;
    this->name = this->executePath = this->command = "";
}

std::string ProcessInfo::getName()
{
    if (this->name.length() == 0)
        this->_readProcessInfoFile(ProcessStatusInfoLine::NAME);
    return this->name;
}

pid_t ProcessInfo::getPid()
{
    return this->pid;
}

pid_t ProcessInfo::getParentPid()
{
    if (this->parentPid < 0)
        this->_readProcessInfoFile(ProcessStatusInfoLine::PARENT_PID);
    return this->parentPid;
}

uid_t ProcessInfo::getUid()
{
    if (this->uid == UINT_MAX)
        this->_readProcessInfoFile(ProcessStatusInfoLine::UID);
    return this->uid;
}

gid_t ProcessInfo::getGid()
{
    if (this->gid == UINT_MAX)
        this->_readProcessInfoFile(ProcessStatusInfoLine::GID);
    return this->gid;
}

std::string ProcessInfo::getExecutePath()
{
    if (this->executePath.length() > 0)
        return this->executePath;

    std::string execPathFilename = this->processEntryDirname + "/exe";

    char execPath[200];
    ssize_t len = readlink(execPathFilename.c_str(), execPath, sizeof(execPath) - 1);
    if (len != -1)
    {
        execPath[len] = '\0';
        this->executePath = std::string(execPath);
    }
    return this->executePath;
}

std::string ProcessInfo::getCommand()
{
    if (this->command.length() > 0)
        return this->command;

    std::ifstream cmdLineFile(this->processEntryDirname + "/cmdline");
    if (!cmdLineFile.is_open())
        return "";
    
    std::getline(cmdLineFile, this->command);
    return this->command;
}

ulong ProcessInfo::getVirtualMemoryUsage()
{
    if (this->virtualMemoryUsage == ULONG_MAX)
        this->_readProcessInfoFile(ProcessStatusInfoLine::VM_SIZE);
    return this->virtualMemoryUsage;
}

ulong ProcessInfo::getPhysicalMemoryUsage()
{
    if (this->physicalMemoryUsage == ULONG_MAX)
        this->_readProcessInfoFile(ProcessStatusInfoLine::RSS);
    return this->physicalMemoryUsage;
}

ulong ProcessInfo::getCpuTime()
{
    if (this->cpuTime != ULONG_MAX)
        return this->cpuTime;

    std::ifstream statFile(this->processEntryDirname + "/stat");
    if (!statFile.is_open())
        return 0;

    std::string line;
    std::getline(statFile, line);

    short index = 0;
    ulong sysCpuTime;
    ulong userCpuTime;

    std::string value;

    for (char c : line)
    {
        if (c != ' ')
        {
            value.push_back(c);
            continue;
        }

        if (index == 13) // 13th word represents for user cpu time
            userCpuTime = std::stoul(value);
        else if (index == 14) // 14th word represents for system cpu time
            sysCpuTime = std::stoul(value);
        else if (index > 14)
            break;
        index++;
        value.clear();
    }

    this->cpuTime = (double)(sysCpuTime + userCpuTime) / CLOCK_PER_MILISECS;
    return this->cpuTime;
}

// TODO
float ProcessInfo::getCpuUtilization()
{
    if (this->cpuUtilization >= 0)
        return this->cpuUtilization;
    return 0;
}

// TODO
double ProcessInfo::getNetworkInBandwidth()
{
    if (this->networkInBandwidth >= 0)
        return this->networkInBandwidth;
    return 0;
}

// TODO
double ProcessInfo::getNetworkOutBandwidth()
{
    if (this->networkOutBandwidth >= 0)
        return this->networkOutBandwidth;
    return 0;
}

// TODO
ulong ProcessInfo::getIoRead()
{
    if (this->ioRead != ULONG_MAX)
        return this->ioRead;
    return 0;
}

// TODO
ulong ProcessInfo::getIoWrite()
{
    if (this->ioWrite != ULONG_MAX)
        return this->ioWrite;
    return 0;
}

void ProcessInfo::print()
{
    std::cout << "Name: " << this->name << std::endl;
    std::cout << "Pid: " << this->pid << std::endl;
    std::cout << "Parent Pid: " << this->parentPid << std::endl;
    std::cout << "Uid: " << this->uid << std::endl;
    std::cout << "Gid: " << this->gid << std::endl;
    std::cout << "Virtual memory usage: " << this->virtualMemoryUsage << " KB" << std::endl;
    std::cout << "Physical memory usage: " << this->physicalMemoryUsage << " KB" << std::endl;
    std::cout << "Cpu time: " << this->cpuTime << " ms" << std::endl;
    std::cout << "Execute path: " << this->executePath << std::endl;
    std::cout << "Command: " << this->command << std::endl;
}

#endif