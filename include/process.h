#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "main.h"

#define CLOCK_PER_MILISECS 1000.0
#define BUFFER_SIZE 128

#define N_PID "pid"
#define N_PPID "parentPid"
#define N_UID "uid"
#define N_GID "gid"
#define N_EXEC_PATH "pid"
#define N_CMD "command"
#define N_VMU "virtualMemoryUsage"
#define N_PMU "physicalMemoryUsage"
#define N_CPUT "cpuTime"
#define N_CPUU "cpuUsage"
#define N_NETIN "networkInBandwidth"
#define N_NETOUT "networkOutBandwidth"
#define N_IOR "ioRead"
#define N_IOW "ioWrite"
#define N_NAME "name"
 
enum ProcessStatusInfoLine
{
    NAME = 0,
    PID = 5,
    PARENT_PID = 6,
    UID = 8,
    GID = 9,
    VM_SIZE = 17,
    RSS = 21
};

inline bool fileExists(const std::string &);

// TODO: Reuse file pointer
class Process
{
private:
    std::string name;
    std::string pid;
    std::string parentPid;
    std::string uid;
    std::string gid;
    std::string executePath;
    std::string command;
    std::string networkInBandwidth; // What interface ???
    std::string networkOutBandwidth;
    std::string ioWrite; // In KB
    std::string ioRead;  // In KB
private:
    std::string statusFilename;
    std::string processEntryDirname;

public:
    Process(pid_t);
    std::string getName();
    std::string getPid();
    std::string getParentPid();
    std::string getUid();
    std::string getGid();
    std::string getExecutePath();
    std::string getCommand();

    std::string getVirtualMemoryUsage();  // In KB
    std::string getPhysicalMemoryUsage(); // In KB
    std::string getCpuTime();            // In ms
    std::string getCpuUsage();            // In %
    std::string getNetworkInBandwidth();
    std::string getNetworkOutBandwidth();
    std::string getIoWrite();
    std::string getIoRead();

private:
    std::string _readProcessInfoFile(const ProcessStatusInfoLine lineNumber)
    {
        std::ifstream processInfoFile(this->statusFilename);
        std::string line;
        std::string value;
        short idx = 0;

        while (std::getline(processInfoFile, line))
        {
            if (idx == lineNumber)
            {
                value = this->_getFirstValueInLine(line);
                break;
            }
            idx++;
        }

        processInfoFile.close();
        return value;
    }

    /**
     * @brief Remove the label appears in the head of line, then get the first value next.\\
     * @brief Ex: "Name: 15 <...values>" -> "15"
     * @param line
     * @return uint
     */
    inline std::string _getFirstValueInLine(std::string &line) const
    {
        std::stringstream ss(line);
        std::string token = "";
        int wordIndex = 0;

        while (ss >> token)
        {
            if (wordIndex == 1)
                return token;
            wordIndex++;
        }

        return line;
    }
};

#endif