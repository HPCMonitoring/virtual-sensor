#include "main.h"

int main()
{
    ProcessInfo processInfo = ProcessInfo(3216);
    processInfo.getGid();
    processInfo.print();
    return 0;
}
