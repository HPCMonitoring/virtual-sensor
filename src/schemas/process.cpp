#include "schemas/process.h"
#include "exceptions.h"


inline bool fileExists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

inline std::string fromBytesToKBs(double number)
{
    std::ostringstream oss;
    oss.precision(0);
    oss << std::fixed << std::round(number / 1024);
    return oss.str();
}

Process::Process(pid_t pid)
{
    if (pid < 0)
        throw InvalidPID();

    this->pid = std::to_string(pid);
    this->entryDirname = "/proc/" + this->pid;
    this->netNs = "bkvspr-" + this->pid;
    if (!fileExists(this->entryDirname + "/status"))
    {
        this->_exists = false;
        return;
    }

    this->_exists = true;
}

bool Process::exists() const
{
    return this->_exists;
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
            NULLIFY(this->executePath);
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
        this->command = std::regex_replace(this->command, std::regex("\""), "\\\"");
        NULLIFY(this->command);
        ifs.close();
    }

    return this->command;
}

std::string Process::getVirtualMemory()
{
    if (this->virtualMemory.length() == 0)
    {
        this->_readStatmFile();
        NULLIFY(this->virtualMemory);
    }
    return this->virtualMemory;
}

std::string Process::getPhysicalMemory()
{
    if (this->physicalMemory.length() == 0)
    {
        this->_readSmapsRollupFile();
        NULLIFY(this->physicalMemory);
    }
    return this->physicalMemory;
}

std::string Process::getCpuTime()
{
    if (this->cpuTime.length() == 0)
    {
        this->_readStatFile();
        NULLIFY(this->cpuTime);
    }
    return this->cpuTime;
}

std::string Process::getCpuUsage()
{
    if (this->cpuUsage.length() > 0)
        return this->cpuUsage;

    std::string command = "ps -p " + this->pid + " -o \%cpu";
    FILE *pipe = popen(command.c_str(), "r");

    char buffer[100];
    short lineNumber = 0;

    while (!feof(pipe) && fgets(buffer, 20, pipe) != nullptr)
    {
        if (lineNumber == 1)
        {
            this->cpuUsage.append(buffer);
            break;
        }
        lineNumber++;
    }

    pclose(pipe);
    if (this->cpuUsage.length() > 2)
        this->cpuUsage.pop_back();   // Remove \n character
    if (this->cpuUsage.length() > 0) // String can be empty if no CPU usage
        this->cpuUsage.erase(0, 1);  // Remove whitespace character

    NULLIFY(this->cpuUsage);
    return this->cpuUsage;
}

std::string Process::getReadKBs()
{
    if (this->readKBs.length() == 0)
    {
        this->_readIoFile();
        NULLIFY(this->readKBs);
    }
    return this->readKBs;
}

std::string Process::getWriteKBs()
{
    if (this->writeKBs.length() == 0)
    {
        this->_readIoFile();
        NULLIFY(this->writeKBs);
    }
    return this->writeKBs;
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
        if (count == 3) // ppid
            this->parentPid = token;
        else if (count == 14) // utime
            utime = std::stoul(token);
        else if (count == 15) // stime
        {
            stime = std::stoul(token);
            break; // No need to continue parsing
        }
        count++;
    }
    std::ostringstream oss;
    oss.precision(3);
    oss << std::fixed << (double)(utime + stime) / CLOCK_PER_MILISECS;
    this->cpuTime = oss.str();

    statFile.close();
}

std::string Process::getNetworkIn()
{
    if (this->networkIn.length() == 0)
    {
        this->_readNetworkStat();
        NULLIFY(this->networkIn);
    }
    return this->networkIn;
}

std::string Process::getNetworkOut()
{
    if (this->networkOut.length() == 0)
    {
        this->_readNetworkStat();
        NULLIFY(this->networkOut);
    }
    return this->networkOut;
}

inline void Process::_setUpNetNs()
{
    const std::string netNsPath = "/var/run/netns/" + this->netNs;
    if (fileExists(netNsPath))
        return;
    symlink((this->entryDirname + "ns/net").c_str(), netNsPath.c_str());
}

inline void Process::_readNetworkStat()
{
    this->_setUpNetNs();

    const std::string cmd = "ip netns exec " + this->netNs + " ifconfig -a | grep -E \'RX packets|TX packets\'";
    FILE *pipe = popen(cmd.c_str(), "r");
    auto logger = SensorLogger::getInstance()->getLogger();

    unsigned long long totalRxBytes = 0, totalTxBytes = 0;
    if (!pipe)
        SPDLOG_LOGGER_ERROR(logger, "Debug: {}", "Network namespace not exists or device not found!");
    else
    {
        char line[512];
        while (fgets(line, sizeof(line), pipe))
        {
            char type[4];
            unsigned long long bytes;
            // TX packets 1929747  bytes 481364580 (481.3 MB)
            if (sscanf(line, "%s %*s %*s %*s %llu %*s %*s", type, &bytes) == 1)
            {
                if (type == "RX")
                    totalRxBytes += bytes;
                else if (type == "TX")
                    totalTxBytes += bytes;
            }
        }

        pclose(pipe);
    }

    this->networkIn = fromBytesToKBs((double)totalRxBytes);  // convert to KBs
    this->networkOut = fromBytesToKBs((double)totalTxBytes); // convert to KBs
}

inline void Process::_readCommFile()
{
    std::ifstream ifs(this->entryDirname + "/comm");
    std::getline(ifs, this->name);
    NULLIFY(this->name);
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

            NULLIFY(token);

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
    NULLIFY(this->virtualMemory);
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

    NULLIFY(this->physicalMemory);

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
            iss >> readBytes;
        else if (field == "write_bytes:")
        {
            iss >> writeBytes;
            break; // No need to continue
        }
    }

    this->readKBs = fromBytesToKBs(readBytes);
    this->writeKBs = fromBytesToKBs(writeBytes);
    ioFile.close();
}