#ifndef __MESSAGE_PRODUCER_H__
#define __MESSAGE_PRODUCER_H__

#include "main.h"
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
    Worker *createWorker(const std::string &topicName, Filter *filter, const time_t interval);
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
        Filter *filter;
        time_t interval;
        std::thread job;
        std::atomic<bool> stopFlag;

    public:
        Worker(RdKafka::Producer *, const std::string &, Filter *, const time_t );
        Filter *getFilter();
        time_t getInterval();
        ~Worker();

    private:
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
    };
};

#endif