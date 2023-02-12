#include "process.h"

int main()
{
    try
    {
        ProcessInfo processInfo = ProcessInfo(-1);
        processInfo.getGid();
        processInfo.print();
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
