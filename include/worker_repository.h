#ifndef VIRTUAL_SENSOR_WORKER_REPOSITORY_H
#define VIRTUAL_SENSOR_WORKER_REPOSITORY_H

#include <list>
#include <mutex>
#include "recorder.h"

class WorkerRepository {
protected:
    static WorkerRepository* _singleton;
    static std::mutex _singletonMutex;
    std::unordered_map<std::string, Recorder*> producers;
    WorkerRepository();
public:
    static WorkerRepository *getInstance();

    WorkerRepository(WorkerRepository &other) = delete;
    void operator=(const WorkerRepository &) = delete;

    void clearAll();
    void addWorker(std::string brokerUrl, Recorder::WorkerProp *);

};
#endif //VIRTUAL_SENSOR_WORKER_REPOSITORY_H
