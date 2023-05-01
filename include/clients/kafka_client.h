#ifndef __KAFKA_CLIENT_H__
#define __KAFKA_CLIENT_H__

#include <librdkafka/rdkafkacpp.h>
#include "repository/filter.h"
#include <iostream>
#include "exceptions.h"
#include "repository/repository.h"

class KakfaClient
{
public:
    class Worker;
    class WorkerProp;

private:
    std::shared_ptr<RdKafka::Producer>producer;
    // Hashmap topic name -> worker
    std::unordered_map<std::string, Worker *> workers;
    KakfaClient(const KakfaClient &) = delete;

public:
    KakfaClient(const std::string &clientId, const std::string &brokerUrl);
    Worker *addWorker(WorkerProp *prop);
    Worker *getWorker(const std::string &topicName) const;
    ~KakfaClient();

public:
    class Worker
    {
    private:
        std::shared_ptr<RdKafka::Producer> handler;
        RdKafka::Topic *topic;
        WorkerProp* prop;
        std::thread job;
        std::atomic<bool> stopFlag;

    public:
        Worker(std::shared_ptr<RdKafka::Producer>, WorkerProp *prop);
        void stop();
        ~Worker();

    private:
        void _sendMessage();

    };

    class WorkerProp {
    public:
        const std::string topicName;
        const Filter *filter;
        const time_t interval;
        WorkerProp(const std::string &topicName, Filter* filter, const time_t interval);
        ~WorkerProp();
    };
};

#endif