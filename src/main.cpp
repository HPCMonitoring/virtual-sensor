#include "sensor.h"

int main(int argc, char *argv[])
{
    std::string credentialPath;

    if (argc == 1)
    {
        credentialPath = "/usr/share/bkvsensor";
    }
    else
    {
        credentialPath = std::string(argv[1]);
    }

    Sensor sensor = Sensor(credentialPath);
    sensor.connect();

    MessageProducer* producer =  sensor.createMsgProducer("bkvsensor", "localhost:9092");
    producer->createWorker("purchases", "SELECT * FROM PROCESS", 10);

    return 0;
}
