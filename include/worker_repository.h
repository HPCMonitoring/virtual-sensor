#ifndef __VIRTUAL_SENSOR_WORKER_REPOSITORY_H__
#define __VIRTUAL_SENSOR_WORKER_REPOSITORY_H__

#include "clients/kafka_client.h"

class WorkerRepository {
protected:
    static WorkerRepository* _singleton;
    static std::mutex _singletonMutex;
    std::unordered_map<std::string, KakfaClient*> producers;
    WorkerRepository();
public:
    static WorkerRepository *getInstance();

    WorkerRepository(WorkerRepository &other) = delete;
    void operator=(const WorkerRepository &) = delete;

    void clearAll();
    void addWorker(std::string brokerUrl, KakfaClient::WorkerProp *);

};
#endif //VIRTUAL_SENSOR_WORKER_REPOSITORY_H
