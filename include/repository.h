#ifndef __PROCESS_REPOSITORY_H__
#define __PROCESS_REPOSITORY_H__

#include "main.h"
#include "filter.h"

#define PROC_FS "/proc"

bool isProcessDirectory(const std::string& name);

class Repository
{
private:
    Repository(){};
    ~Repository(){};
    // Mark as ProcessRepository does not have copy constructor & and cannot assign to another variables
    Repository(const Repository &) = delete;
    Repository &operator=(const Repository &) = delete;

public:
    static Repository &getInstance();
    std::vector<std::string> getData(const Filter*);

private:
    std::vector<pid_t> getAllPids()
    {
        std::vector<pid_t> pids;

        DIR *procDir = opendir(PROC_FS); // Open the /proc directory
        if (!procDir)
        {
            std::cerr << "Error: could not open /proc directory\n";
            return pids;
        }

        dirent *entry = readdir(procDir);
        while (entry != NULL)
        {
            std::string name = entry->d_name;
            if (entry->d_type == DT_DIR && isProcessDirectory(name))
            {
                const pid_t pid = std::atoi(name.c_str());
                pids.push_back(pid);
            }
            entry = readdir(procDir);
        }
        closedir(procDir);
        return pids;
    }
};

#endif