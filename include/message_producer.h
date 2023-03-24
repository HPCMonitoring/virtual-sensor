#ifndef __MESSAGE_PRODUCER_H__
#define __MESSAGE_PRODUCER_H__

#include "main.h"
#include "common.h"
#include "filter.h"

class MessageProducer
{
public:
    class Worker;

private:
    RdKafka::Producer *producer;
    // Hashmap topic name -> worker
    std::unordered_map<std::string, Worker *> workers;
    MessageProducer(const MessageProducer &) = delete;

public:
    MessageProducer(const std::string &clientId, const std::string &brokerUrl);
    Worker *createWorker(const std::string &topicName, Filter* filter, const time_t interval);
    void removeWorker(const std::string &topicName);
    Worker *getWorker(const std::string &topicName) const;
    ~MessageProducer();

public:
    class Worker
    {
    private:
        RdKafka::Producer *handler;
        RdKafka::Topic *topic;
        std::string topicName;
        Filter* filter;
        time_t interval;
        std::thread job;
        std::atomic<bool> stopFlag;

    private:
        Worker(RdKafka::Topic *topic, RdKafka::Producer *handler)
        {
            this->topic = topic;
            this->handler = handler;
            this->stopFlag = false;
        };
        Worker(const Worker &) = delete;
        Worker &operator=(const Worker &) = delete;
        void _sendMessage(const std::string &msg)
        {
            while (stopFlag == false)
            {
                /* code */
                this->handler->produce(this->topic,
                                       RdKafka::Topic::PARTITION_UA,
                                       RdKafka::Producer::RK_MSG_COPY,
                                       const_cast<char *>(msg.c_str()),
                                       msg.size(),
                                       NULL,  // Key
                                       0,     // Key length
                                       NULL); // Opaque value
                sleep(this->interval);
            }
        }

    public:
        static Worker *buildTopic(RdKafka::Producer *producer, const std::string &topicName);
        Worker *buildFilter(Filter*);
        Worker *buildTimeInterval(const time_t interval);
        Worker *build();
        Filter *getFilter();
        time_t getInterval();

        ~Worker();
    };
};

#endif