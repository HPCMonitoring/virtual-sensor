// #include "sensor.h"
#include "filter.h"

void signalHandler(int signal)
{
    std::cout << "Terminating with exit code " << signal << " ...\n";
    exit(signal);
}

int main(int argc, char *argv[])
{
    // Register signal handler
    signal(SIGINT, signalHandler);

    // MessageProducer producer("1915940", "localhost:9092");
    // producer.createWorker("purchases", "", 5);

    Filter filter("process-info", "id name", "|| 2 == uid 10 < cpu 20");
    filter.print();

    // while (true);
    return 0;
}
