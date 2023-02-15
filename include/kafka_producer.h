#ifndef __KAFKA_PRODUCER_H__
#define __KAFKA_PRODUCER_H__

#include "main.h"
#include "common.h"

class KafkaProducer
{
public:
    class Topic;

private:
    std::string clientId;
    std::string broker;
    std::unordered_map<std::string, Topic*> topics;

public:
    KafkaProducer(const std::string &clientId, const std::string &broker);
    void addTopic(const std::string &topic, const FilterRule &filterRule, time_t interval);
    void removeTopic(const std::string &topic);
    void updateTopic(const std::string &topic, const FilterRule &filterRule, time_t interval);
    ~KafkaProducer();

public:
    class Topic
    {
    private:
        std::string topic;
        FilterRule filterRule;
        time_t interval;
        pthread_t tid;

    public:
        Topic(const FilterRule &filterRule, time_t interval)
        {
            this->filterRule = filterRule;
            this->interval = interval;
        }
        // Return a modifiable reference of filter rule
        FilterRule *getFilterRule()
        {
            return &this->filterRule;
        }
        time_t getInterval()
        {
            return this->interval;
        }

    private:
        void _send(std::string topic, std::string message);
    };
};

#endif