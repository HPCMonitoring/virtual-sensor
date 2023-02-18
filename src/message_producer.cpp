#include "message_producer.h"
#include "exceptions.h"
#ifdef __MESSAGE_PRODUCER_H__

MessageProducer::MessageProducer(const std::string &clientId, const std::string &brokerUrl)
{
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    conf->set("client.id", clientId, errstr);
    conf->set("bootstrap.servers", brokerUrl, errstr);

    RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(conf, errstr);

    if (!kafkaProducer)
    {
        throw std::runtime_error(ERR_CREATE_PRODUCER + errstr);
    }

    this->producer = kafkaProducer;
    delete conf;
}

MessageProducer::~MessageProducer()
{
    for (std::unordered_map<std::string, MessageProducer::Worker *>::iterator i = this->workers.begin(); i != this->workers.end(); i++)
    {
        delete i->second;
    }
    this->producer->flush(5000);
    delete this->producer;
}

MessageProducer::Worker *MessageProducer::createWorker(const std::string &topicName, const FilterRule &filterRule, const time_t interval)
{
    MessageProducer::Worker *worker = MessageProducer::Worker::buildTopic(this->producer, topicName)
                                          ->buildFilterRule(filterRule)
                                          ->buildTimeInterval(interval)
                                          ->build();
    this->workers.insert({topicName, worker});
    return worker;
}

void MessageProducer::removeWorker(const std::string &topicName)
{
    MessageProducer::Worker *worker = this->workers.at(topicName);
    delete worker;
    this->workers.erase(topicName);
}

MessageProducer::Worker *MessageProducer::getWorker(const std::string &topicName)
{
    return this->workers.at(topicName);
}

MessageProducer::Worker *MessageProducer::Worker::buildTopic(RdKafka::Producer *producer, const std::string &topicName)
{
    std::string errMsg;
    RdKafka::Topic *rawTopic = RdKafka::Topic::create(producer, topicName, NULL, errMsg);
    MessageProducer::Worker *topic = new Worker(rawTopic, producer);
    topic->topicName = topicName;
    return topic;
}

MessageProducer::Worker *MessageProducer::Worker::buildFilterRule(const FilterRule &filterRule)
{
    this->filterRule = filterRule;
    return this;
}

MessageProducer::Worker *MessageProducer::Worker::buildTimeInterval(const time_t interval)
{
    this->interval = interval;
    return this;
}

MessageProducer::Worker *MessageProducer::Worker::build()
{
    this->_sendMessage("Hello world !");
    return this;
}

FilterRule *MessageProducer::Worker::getFilterRule()
{
    return &this->filterRule;
}

time_t MessageProducer::Worker::getInterval()
{
    return this->interval;
}

MessageProducer::Worker::~Worker()
{
    delete this->topic;
}

#endif