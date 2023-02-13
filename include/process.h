#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "main.h"

#define CLOCK_PER_MILISECS 1000.0
#define BUFFER_SIZE 128

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
class ProcessInfo
{
private:
    std::string name;
    pid_t pid;
    pid_t parentPid;
    uid_t uid;
    gid_t gid;
    std::string executePath;
    std::string command;
    double networkInBandwidth; // What interface ???
    double networkOutBandwidth;
    ulong ioWrite; // In KB
    ulong ioRead;  // In KB
private:
    std::string statusFilename;
    std::string processEntryDirname;

public:
    ProcessInfo(pid_t);
    std::string getName();

    pid_t getPid();
    pid_t getParentPid();
    uid_t getUid();
    gid_t getGid();
    std::string getExecutePath();
    std::string getCommand();

    ulong getVirtualMemoryUsage();  // In KB
    ulong getPhysicalMemoryUsage(); // In KB
    double getCpuTime();            // In ms
    float getCpuUsage();            // In %
    double getNetworkInBandwidth();
    double getNetworkOutBandwidth();
    ulong getIoWrite();
    ulong getIoRead();
    void print();

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

void printProcessInfo(ProcessInfo *);

#endif