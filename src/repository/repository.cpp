#include "repository/repository.h"

bool isProcessDirectory(const std::string &name)
{
    // Check if the directory name consists entirely of digits
    const size_t length = name.size();
    for (size_t i = 0; i < length; ++i)
    {
        if (name.at(i) < '0' || name.at(i) > '9')
        {
            return false;
        }
    }
    return true;
}

Repository &Repository::getInstance()
{
    static Repository instance;
    return instance;
}

std::vector<std::string> Repository::getData(const Filter *filter)
{
    std::vector<std::string> results;

    if (filter->datatype == PROCESS)
    {
        const std::string pidLiteral = filter->selection != nullptr ? filter->selection->findPid() : "";
        if (pidLiteral.length() > 0)
        {
            const pid_t pid = std::stoi(pidLiteral);
            Process proc(pid);
            if (proc.exists() && filter->selection->validate(&proc))
                results.push_back(filter->iterateProc(&proc));
        }
        else
        {
            std::vector<pid_t> allPids = this->getAllPids();
            const size_t numOfProcesses = allPids.size();
            for (size_t i = 0; i < numOfProcesses; ++i)
            {
                Process proc(allPids.at(i));
                if (!proc.exists() || !filter->selection->validate(&proc))
                    continue;

                results.push_back(filter->iterateProc(&proc));
            }
        }
    }
    else if (filter->datatype == NETWORK_INTERFACE)
    {
        std::ifstream netfile("/proc/net/dev");
        if (!netfile.is_open())
            return results;

        std::string line;
        std::getline(netfile, line); // skip first line
        std::getline(netfile, line); // skip second line

        while (std::getline(netfile, line))
        {
            NetworkInterface networkInterface(line);
            results.push_back(filter->iterateNetworkInterface(&networkInterface));
        }
        netfile.close();
    }
    else if (filter->datatype == MEMORY)
    {
        Memory memory = Memory();
        results.push_back(filter->iterateMemory(&memory));
    }
    else if (filter->datatype == CPU)
    {
        std::ifstream statfile("/proc/stat");
        if (!statfile.is_open())
            return results;
        std::string line;
        while (std::getline(statfile, line))
        {
            if (line.find("cpu") == 0 && line.find("cpu ") != 0)
            {
                Cpu cpu = Cpu(line);
                results.push_back(filter->iterateCpu(&cpu));
            }
        }
    }
    else if (filter->datatype == IO)
    {
        // Run iostat and read its output
        FILE *pipe = popen("iostat -d -k 1 1", "r");
        if (!pipe)
        {
            std::cerr << "Error: Failed to run iostat command." << std::endl;
            return results;
        }
        char buffer[256];
        std::string iostatOutput;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
            iostatOutput += buffer;

        std::stringstream iss(iostatOutput);
        std::string line;
        while (std::getline(iss, line))
        {
            // Find header line
            if (line.find("Device") == std::string::npos)
                continue;
            break;
        }
        // Parse device IO stats from each line
        while (std::getline(iss, line))
        {
            if(line.empty()) break;
            IOStat io = IOStat(line);
            results.push_back(filter->iterateIO(&io));
        }
    }
    else if (filter->datatype == DISK)
    {
        // Run iostat and read its output
        FILE *pipe = popen("df -h", "r");
        if (!pipe)
        {
            std::cerr << "Error: Failed to run df command." << std::endl;
            return results;
        }
        char buffer[256];
        std::string iostatOutput;
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
            iostatOutput += buffer;

        std::stringstream iss(iostatOutput);
        std::string line;
        // Skip header line
        std::getline(iss, line);
        while(std::getline(iss, line)) {
            DiskUsage disk = DiskUsage(line);
            results.push_back(filter->iterateDiskUsage(&disk));
        }
    }

    return results;
}
