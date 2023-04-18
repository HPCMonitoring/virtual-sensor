#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "main.h"

#define CLOCK_PER_MILISECS 1000.0
#define BUFFER_SIZE 5000

#define N_PID "pid"
#define N_PPID "parentPid"
#define N_UID "uid"
#define N_GID "gid"
#define N_EXEC_PATH "executePath"
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
    std::string virtualMemory;
    std::string physicalMemory;
    std::string cpuTime;
    std::string cpuUsage;
    std::string networkInBandwidth; // What interface ???
    std::string networkOutBandwidth;
    std::string readKBs;  // In KB
    std::string writeKBs; // In KB
    std::string readPerSec;
    std::string writePerSec;
    bool _exists;
    unsigned long long startTime;

private:
    std::string entryDirname;

public:
    Process(pid_t);
    bool exists() const;
    std::string getName();
    std::string getPid();
    std::string getParentPid();
    std::string getUid();
    std::string getGid();
    std::string getExecutePath();
    std::string getCommand();
    std::string getVirtualMemory();  // In KB
    std::string getPhysicalMemory(); // In KB
    std::string getCpuTime();        // In ms
    std::string getCpuUsage();       // In %
    std::string getNetworkInBandwidth();
    std::string getNetworkOutBandwidth();
    std::string getWriteKBs();
    std::string getReadKBs();
    std::string getIOReadPerSec();
    std::string getIOWritePerSec();

private:
    inline unsigned long long _getStartTime();

private:
    inline void _readStatFile();
    inline void _readCommFile();
    inline void _readStatusFile();
    inline void _readStatmFile();
    inline void _readSmapsRollupFile();
    inline void _readIoFile();
};

#endif