#include "schemas/process.h"
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

    this->entryDirname = "/proc/" + std::to_string(pid);
    if (!fileExists(this->entryDirname + "/status"))
    {
        this->_exists = false;
        return;
    }

    this->pid = std::to_string(pid);
    this->startTime = 0;
    this->_exists = true;
}

std::string Process::getName()
{
    if (this->name.length() == 0)
        this->_readCommFile();
    return this->name;
}

std::string Process::getPid()
{
    return this->pid;
}

std::string Process::getParentPid()
{
    if (this->parentPid.length() == 0)
        this->_readStatFile();
    return this->parentPid;
}

std::string Process::getUid()
{
    if (this->uid.length() == 0)
        this->_readStatusFile();
    return this->uid;
}

std::string Process::getGid()
{
    if (this->gid.length() == 0)
        this->_readStatusFile();
    return this->gid;
}

std::string Process::getExecutePath()
{
    if (this->executePath.length() == 0)
    {
        std::string execPathFilename = this->entryDirname + "/exe";

        char execPath[BUFFER_SIZE];
        ssize_t len = readlink(execPathFilename.c_str(), execPath, sizeof(execPath) - 1);
        if (len != -1)
        {
            execPath[len] = '\0';
            this->executePath = std::string(execPath);
        }
    }

    return this->executePath;
}

std::string Process::getCommand()
{
    if (this->command.length() == 0)
    {
        std::ifstream ifs(this->entryDirname + "/cmdline");
        if (!ifs.is_open())
            return "";

        std::getline(ifs, this->command);
        ifs.close();
    }

    return this->command;
}

std::string Process::getVirtualMemory()
{
    if (this->virtualMemory.length() == 0)
        this->_readStatmFile();

    return this->virtualMemory;
}

std::string Process::getPhysicalMemory()
{
    if (this->physicalMemory.length() == 0)
        this->_readSmapsRollupFile();
    return this->physicalMemory;
}

std::string Process::getCpuTime()
{
    if (this->cpuTime.length() == 0)
        this->_readStatFile();
    return this->cpuTime;
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

    if (result.length() != 2)
        result.pop_back(); // Remove \n character
    result.erase(0, 1);    // Remove whitespace character
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

std::string Process::getReadKBs()
{
    if (this->readKBs.length() == 0)
        this->_readIoFile();
    return this->readKBs;
}

std::string Process::getWriteKBs()
{
    if (this->writeKBs.length() == 0)
        this->_readIoFile();
    return this->writePerSec;
}

std::string Process::getIOReadPerSec()
{
    if (this->readPerSec.length() == 0)
    {
        std::string readKBs = this->getReadKBs();
        auto startTime = std::chrono::system_clock::from_time_t(0) + std::chrono::seconds(this->_getStartTime() / sysconf(_SC_CLK_TCK));
        auto currentTime = std::chrono::system_clock::now();
        int64_t elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        double readPerSec = std::stod(readKBs) / elapsedTime;
        this->readPerSec = std::to_string(readPerSec);
    }

    return this->readPerSec;
}

std::string Process::getIOWritePerSec()
{
    if (this->writePerSec.length() == 0)
    {
        std::string writeKBs = this->getWriteKBs();
        auto startTime = std::chrono::system_clock::from_time_t(0) + std::chrono::seconds(this->_getStartTime() / sysconf(_SC_CLK_TCK));
        auto currentTime = std::chrono::system_clock::now();
        int64_t elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        double writePerSec = std::stod(writeKBs) / elapsedTime;
        this->writePerSec = std::to_string(writePerSec);
    }

    return this->writePerSec;
}

inline unsigned long long Process::_getStartTime()
{
    if (this->startTime == 0)
        this->_readStatFile();
    return this->startTime;
}

inline void Process::_readStatFile()
{
    std::ifstream statFile(this->entryDirname + "/stat");
    std::string content;
    std::getline(statFile, content);
    std::istringstream iss(content);
    std::string token;
    int count = 0;

    ulong utime, stime;

    while (iss >> token)
    {
        count++;
        if (count == 3) // ppid
            this->parentPid = token;
        else if (count == 14) // utime
            utime = std::stoul(token);
        else if (count == 15) // stime
            stime = std::stoul(token);
        else if (count == 22) // start_time
        {
            this->startTime = std::stoull(token);
            break; // No need to continue parsing
        }
    }
    this->cpuTime = std::to_string((double)(utime + stime) / CLOCK_PER_MILISECS);

    statFile.close();
}

inline void Process::_readCommFile()
{
    std::ifstream ifs(this->entryDirname + "/comm");
    std::getline(ifs, this->name);
    ifs.close();
}

inline void Process::_readStatusFile()
{
    std::ifstream statusFile(this->entryDirname + "/status");
    std::string line;
    std::string value;
    short idx = 0;

    while (std::getline(statusFile, line))
    {
        if (idx == 8 || idx == 9) // 8 for UID, 9 for GID
        {
            std::stringstream ss(line);
            std::string token;

            ss >> token;
            ss >> token;

            idx == 8 ? this->uid = token : this->gid = token;
        }
        else if (idx > 9)
            break;
        idx++;
    }

    statusFile.close();
}

inline void Process::_readStatmFile()
{
    std::ifstream statmFile(this->entryDirname + "/statm");
    std::getline(statmFile, this->virtualMemory, ' ');
    statmFile.close();
}

inline void Process::_readSmapsRollupFile()
{
    std::ifstream statmFile(this->entryDirname + "/smaps_rollup");
    std::string line;

    std::getline(statmFile, line); // Ignore headers
    std::getline(statmFile, line);

    std::istringstream iss(line);

    iss >> this->physicalMemory; // Ignore "Rss:"
    iss >> this->physicalMemory;

    statmFile.close();
}

inline void Process::_readIoFile()
{
    std::ifstream ioFile(this->entryDirname + "/io");
    std::string line;
    double readBytes = 0, writeBytes = 0;

    while (std::getline(ioFile, line))
    {
        std::istringstream iss(line);
        std::string field;
        iss >> field;

        if (field == "read_bytes:")
        {
            iss >> readBytes;
        }
        else if (field == "write_bytes:")
        {
            iss >> writeBytes;
            break; // No need to continue
        }
    }

    this->readKBs = std::to_string(std::round(readBytes / 1024));
    this->writeKBs = std::to_string(std::round(writeBytes / 1024));

    ioFile.close();
}