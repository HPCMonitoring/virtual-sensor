#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "main.h"



enum ProcessStatusInfoLine
{
    NAME = 0,
    PID = 5,
    PARENT_PID = 6,
    UID = 8,
    GID = 9
};

inline bool fileExists(const std::string &);

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
    ulong memoryUsage; // In KB
    ulong cpuTime;
    float cpuUtilization;      // In %
    double networkInBandwidth; // What interface ???
    double networkOutBandwidth;
    ulong ioWrite; // In KB
    ulong ioRead;  // In KB
private:
    std::string statusFilename;
    std::string executePathFilename;
    std::string commandFilename;

public:
    ProcessInfo(pid_t);
    std::string getName();

    // PID, PPID, UID, GID is in the same file with positions increase respectively. For save execution cost, call by reverse order
    pid_t getPid();
    pid_t getParentPid();
    uid_t getUid();
    gid_t getGid();

    std::string getExecutePath();
    std::string getCommand();
    ulong getMemoryUsage();
    ulong getCpuTime();
    float getCpuUtilization();
    double getNetworkInBandwidth();
    double getNetworkOutBandwidth();
    ulong getIoWrite();
    ulong getIoRead();
    void print();

private:
    void _readProcessInfoFile(const ProcessStatusInfoLine lastLine)
    {
        std::ifstream processInfoFile(this->statusFilename);
        std::string line;
        short lineNumber = 0;

        while (std::getline(processInfoFile, line))
        {
            switch (lineNumber)
            {
            case ProcessStatusInfoLine::NAME: // Read process name
                this->name = this->_getFirstValueInLine(line);
                break;
            case ProcessStatusInfoLine::PARENT_PID: // Read parent PID
                this->parentPid = std::stoi(this->_getFirstValueInLine(line));
                break;
            case ProcessStatusInfoLine::UID: // Read UID
                this->uid = std::stoi(this->_getFirstValueInLine(line));
                break;
            case ProcessStatusInfoLine::GID: // Read GID
                this->gid = std::stoi(this->_getFirstValueInLine(line));
                break;
            default:
                break;
            }
            if (lineNumber == lastLine)
                break;
            lineNumber++;
        }

        processInfoFile.close();
    }

    /**
     * @brief Remove the label appears in the head of line, then get the first value next.\\
     * @brief Ex: "Name: 15 <...values>" -> "15"
     * @param line
     * @return uint
     */
    std::string _getFirstValueInLine(std::string &line) const
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