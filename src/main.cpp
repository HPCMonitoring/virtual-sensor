#include "process.h"

int main()
{
    try
    {
        ProcessInfo processInfo = ProcessInfo(1);
        printProcessInfo(&processInfo);
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
