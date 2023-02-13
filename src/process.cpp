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
    this->networkInBandwidth = this->networkOutBandwidth = -1;
    this->ioRead = this->ioWrite = ULONG_MAX;
    this->name = this->executePath = this->command = "";
}

std::string ProcessInfo::getName()
{
    if (this->name.length() == 0)
        this->name = this->_readProcessInfoFile(ProcessStatusInfoLine::NAME);
    return this->name;
}

pid_t ProcessInfo::getPid()
{
    return this->pid;
}

pid_t ProcessInfo::getParentPid()
{
    if (this->parentPid < 0)
    {
        std::string parentPid = this->_readProcessInfoFile(ProcessStatusInfoLine::PARENT_PID);
        this->parentPid = std::stoi(parentPid);
    }
    return this->parentPid;
}

uid_t ProcessInfo::getUid()
{
    if (this->uid == UINT_MAX)
    {
        std::string uid = this->_readProcessInfoFile(ProcessStatusInfoLine::UID);
        this->uid = std::stoul(uid);
    }
    return this->uid;
}

gid_t ProcessInfo::getGid()
{
    if (this->gid == UINT_MAX)
    {
        std::string gid = this->_readProcessInfoFile(ProcessStatusInfoLine::GID);
        this->gid = std::stoul(gid);
    }
    return this->gid;
}

std::string ProcessInfo::getExecutePath()
{
    if (this->executePath.length() > 0)
        return this->executePath;

    std::string execPathFilename = this->processEntryDirname + "/exe";

    char execPath[BUFFER_SIZE];
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
    std::string virtualMemoryUsage = this->_readProcessInfoFile(ProcessStatusInfoLine::VM_SIZE);
    return std::stoul(virtualMemoryUsage);
}

ulong ProcessInfo::getPhysicalMemoryUsage()
{
    std::string physicalMemoryUsage = this->_readProcessInfoFile(ProcessStatusInfoLine::RSS);
    return std::stoul(physicalMemoryUsage);
}

double ProcessInfo::getCpuTime()
{
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

    return (double)(sysCpuTime + userCpuTime) / CLOCK_PER_MILISECS;
}

float ProcessInfo::getCpuUsage()
{
    std::string command = "ps -p " + std::to_string(this->pid) + " -o \%cpu";
    FILE *pipe = popen(command.c_str(), "r");

    char buffer[BUFFER_SIZE];
    std::string result = "";
    short lineNumber = 0;

    while (!feof(pipe) && fgets(buffer, BUFFER_SIZE, pipe) != NULL)
    {
        if (lineNumber == 1)
        {
            result += buffer;
            break;
        }
        lineNumber++;
    }
    pclose(pipe);

    result.pop_back();  // Remove \n character
    result.erase(0, 1); // Remove whitespace character

    return std::stof(result);
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

void printProcessInfo(ProcessInfo* p) {
    std::cout << "Name: " << p->getName() << std::endl;
    std::cout << "Pid: " << p->getPid() << std::endl;
    std::cout << "Parent Pid: " << p->getParentPid() << std::endl;
    std::cout << "Uid: " << p->getUid() << std::endl;
    std::cout << "Gid: " << p->getGid() << std::endl;
    std::cout << "Virtual memory usage: " << p->getVirtualMemoryUsage() << " KB" << std::endl;
    std::cout << "Physical memory usage: " << p->getPhysicalMemoryUsage() << " KB" << std::endl;
    std::cout << "Cpu time: " << p->getCpuTime() << " ms" << std::endl;
    std::cout << "Cpu usage: " << p->getCpuUsage() << " %" << std::endl;
    std::cout << "Execute path: " << p->getExecutePath() << std::endl;
    std::cout << "Command: " << p->getCommand() << std::endl;
}

#endif