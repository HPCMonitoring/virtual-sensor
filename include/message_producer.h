#ifndef __MESSAGE_PRODUCER_H__
#define __MESSAGE_PRODUCER_H__

#include "main.h"
#include "common.h"

class MessageProducer
{
public:
    class TopicWithFilterRule;

private:
    RdKafka::Producer* producer;
    RdKafka::Conf* conf;
    std::unordered_map<std::string, TopicWithFilterRule*> topics;
    MessageProducer(const MessageProducer&) = delete;

public:
    MessageProducer(const std::string &clientId, const std::string &brokerUrl);
    void addTopic(const std::string &topicName, const FilterRule &filterRule, time_t interval);
    void removeTopic(const std::string &topicName);
    void updateTopic(const std::string &topicName, const FilterRule &filterRule, time_t interval);
    ~MessageProducer();
    void send(const std::string &topicName, const std::string&);

public:
    class TopicWithFilterRule
    {
    private:
        RdKafka::Topic* topic;
        FilterRule filterRule;
        time_t interval;
    private:
        TopicWithFilterRule(RdKafka::Topic* topic) {
            this->topic = topic;
        };
    public:
        static MessageProducer::TopicWithFilterRule* build(RdKafka::Producer* producer, const std::string& topicName)
        {
            std::string errMsg;
            RdKafka::Topic* rawTopic = RdKafka::Topic::create(producer, topicName, NULL, errMsg);
            MessageProducer::TopicWithFilterRule* topic = new TopicWithFilterRule(rawTopic);
            return topic;
        }

        MessageProducer::TopicWithFilterRule* buildFilterRule(const FilterRule& filterRule) {
            this->filterRule = filterRule;
            return this;
        }

        MessageProducer::TopicWithFilterRule* buildTimeInterval(const time_t interval) {
            this->interval = interval;
            return this;
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
    };
};

#endif