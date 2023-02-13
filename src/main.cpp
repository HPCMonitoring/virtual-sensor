#include "process.h"

int main()
{
    try
    {
        ProcessInfo processInfo = ProcessInfo(16697);
        processInfo.getPhysicalMemoryUsage();
        processInfo.getCpuTime();
        processInfo.getExecutePath();
        processInfo.getCommand();
        processInfo.print();
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
