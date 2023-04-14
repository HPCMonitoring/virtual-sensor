#ifndef __MESSAGE_PRODUCER_H__
#define __MESSAGE_PRODUCER_H__

#include "main.h"
#include "filter.h"

class Recorder
{
public:
    class Worker;
    class WorkerProp;

private:
    RdKafka::Producer *producer;
    // Hashmap topic name -> worker
    std::unordered_map<std::string, Worker *> workers;
    Recorder(const Recorder &) = delete;

public:
    Recorder(const std::string &clientId, const std::string &brokerUrl);
    Worker *addWorker(WorkerProp *prop);
    Worker *getWorker(const std::string &topicName) const;
    ~Recorder();

public:
    class Worker
    {
    private:
        RdKafka::Producer *handler;
        RdKafka::Topic *topic;
        WorkerProp* prop;
        std::thread job;
        std::atomic<bool> stopFlag;

    public:
        Worker(RdKafka::Producer *, WorkerProp *);
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