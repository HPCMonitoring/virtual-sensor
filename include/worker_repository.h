#ifndef VIRTUAL_SENSOR_WORKER_REPOSITORY_H
#define VIRTUAL_SENSOR_WORKER_REPOSITORY_H

#include <list>
#include <mutex>
#include "message_producer.h"

class WorkerRepository {
protected:
    static WorkerRepository* _singleton;
    static std::mutex _singletonMutex;
    std::unordered_map<std::string, MessageProducer*> producers;
    WorkerRepository();
public:
    static WorkerRepository *getInstance();

    WorkerRepository(WorkerRepository &other) = delete;
    void operator=(const WorkerRepository &) = delete;

    void clearAll();
    void addWorker(std::string brokerUrl, MessageProducer::WorkerProp *);

};
#endif //VIRTUAL_SENSOR_WORKER_REPOSITORY_H
