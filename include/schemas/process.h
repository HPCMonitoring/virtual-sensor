#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "main.h"

#define CLOCK_PER_MILISECS 1000.0
#define BUFFER_SIZE 5000

#define N_NAME "name"
#define N_PID "pid"
#define N_PPID "parentPid"
#define N_UID "uid"
#define N_GID "gid"
#define N_EXEC_PATH "executePath"
#define N_CMD "command"
#define N_VMU "virtualMemory"
#define N_PMU "physicalMemory"
#define N_CPUT "cpuTime"
#define N_CPUU "cpuUsage"
#define N_NETIN "networkInBandwidth"
#define N_NETOUT "networkOutBandwidth"
#define N_IOR "readKBs"
#define N_IOW "writeKBs"

inline bool fileExists(const std::string &);

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
    bool _exists;

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

private:
    inline void _readStatFile();
    inline void _readCommFile();
    inline void _readStatusFile();
    inline void _readStatmFile();
    inline void _readSmapsRollupFile();
    inline void _readIoFile();
};

#endif