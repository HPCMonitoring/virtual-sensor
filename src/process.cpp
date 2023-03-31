#include "process.h"
#include "exceptions.h"

inline bool fileExists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

Process::Process(pid_t pid)
{
    if (pid < 0)
        throw InvalidPID();

    std::string processEntryDirname = "/proc/" + std::to_string(pid);
    this->statusFilename = processEntryDirname + "/status";
    if (!fileExists(this->statusFilename))
        throw ProcessNotExists();

    this->processEntryDirname = processEntryDirname;

    this->pid = std::to_string(pid);
}

std::string Process::getName()
{
    if (this->name.length() == 0)
        this->name = this->_readProcessInfoFile(ProcessStatusInfoLine::NAME);
    return this->name;
}

std::string Process::getPid()
{
    return this->pid;
}

std::string Process::getParentPid()
{
    if (this->parentPid.length() == 0)
        this->parentPid = this->_readProcessInfoFile(ProcessStatusInfoLine::PARENT_PID);
    return this->parentPid;
}

std::string Process::getUid()
{
    if (this->uid.length() == 0)
        this->uid = this->_readProcessInfoFile(ProcessStatusInfoLine::UID);
    return this->uid;
}

std::string Process::getGid()
{
    if (this->gid.length() == 0)
        this->gid = this->_readProcessInfoFile(ProcessStatusInfoLine::GID);
    return this->gid;
}

std::string Process::getExecutePath()
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

std::string Process::getCommand()
{
    if (this->command.length() > 0)
        return this->command;

    std::ifstream cmdLineFile(this->processEntryDirname + "/cmdline");
    if (!cmdLineFile.is_open())
        return "";

    std::getline(cmdLineFile, this->command);
    return this->command;
}

std::string Process::getVirtualMemoryUsage()
{
    return this->_readProcessInfoFile(ProcessStatusInfoLine::VM_SIZE);
}

std::string Process::getPhysicalMemoryUsage()
{
    return this->_readProcessInfoFile(ProcessStatusInfoLine::RSS);
}

std::string Process::getCpuTime()
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

    return std::to_string((double)(sysCpuTime + userCpuTime) / CLOCK_PER_MILISECS);
}

std::string Process::getCpuUsage()
{
    std::string command = "ps -p " + this->pid + " -o \%cpu";
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
    return result;
}

// TODO
std::string Process::getNetworkInBandwidth()
{
    return "-1";
}

// TODO
std::string Process::getNetworkOutBandwidth()
{
    return "-1";
}

// TODO
std::string Process::getIoRead()
{

    return "-1";
}

// TODO
std::string Process::getIoWrite()
{
    return "-1";
}

std::string Process::toJsonStr(const std::vector<Attribute> &attrs)
{
    size_t numOfAttrs = attrs.size();
    std::string result;
    result.push_back('{');

    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string jsonAttrStr = "";
        std::string fieldName = attrs.at(i).alias.length() > 0 ? attrs.at(i).alias : attrs.at(i).name;

        jsonAttrStr.push_back('\"');
        jsonAttrStr.append(fieldName);
        jsonAttrStr.append("\":");

        if (attrs.at(i).name == "name")
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(this->getName());
            jsonAttrStr.push_back('\"');
        }
        else if (attrs.at(i).name == "pid")
            jsonAttrStr.append(this->getPid());
        else if (attrs.at(i).name == "parentPid")
            jsonAttrStr.append(this->getParentPid());
        else if (attrs.at(i).name == "uid")
            jsonAttrStr.append(this->getUid());
        else if (attrs.at(i).name == "gid")
            jsonAttrStr.append(this->getGid());
        else if (attrs.at(i).name == "executePath")
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(this->getExecutePath());
            jsonAttrStr.push_back('\"');
        }
        else if (attrs.at(i).name == "command")
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(this->getCommand());
            jsonAttrStr.push_back('\"');
        }
        else if (attrs.at(i).name == "virtualMemoryUsage")
            jsonAttrStr.append(this->getVirtualMemoryUsage());
        else if (attrs.at(i).name == "physicalMemoryUsage")
            jsonAttrStr.append(this->getPhysicalMemoryUsage());
        else if (attrs.at(i).name == "cpuTime")
            jsonAttrStr.append(this->getCpuTime());
        else if (attrs.at(i).name == "cpuUsage")
            jsonAttrStr.append(this->getCpuUsage());
        else if (attrs.at(i).name == "networkInBandwidth")
            jsonAttrStr.append(this->getNetworkInBandwidth());
        else if (attrs.at(i).name == "networkOutBandwidth")
            jsonAttrStr.append(this->getNetworkOutBandwidth());
        else if (attrs.at(i).name == "ioRead")
            jsonAttrStr.append(this->getIoRead());
        else if (attrs.at(i).name == "ioWrite")
            jsonAttrStr.append(this->getIoWrite());

        if (i != numOfAttrs - 1)
            jsonAttrStr.push_back(',');

        result.append(jsonAttrStr);
    }

    result.push_back('}');
    return result;
}