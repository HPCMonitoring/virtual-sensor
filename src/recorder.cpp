#include "recorder.h"
#include "exceptions.h"

Recorder::Recorder(const std::string &clientId, const std::string &brokerUrl)
{
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("client.id", clientId, errstr);
    conf->set("bootstrap.servers", brokerUrl, errstr);

    RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(conf, errstr);

    if (!kafkaProducer)
        throw std::runtime_error(ERR_CREATE_PRODUCER + errstr);

    this->producer = kafkaProducer;
    delete conf;
}

Recorder::~Recorder()
{
    std::unordered_map<std::string, Recorder::Worker *>::iterator i;
    for (i = this->workers.begin(); i != this->workers.end(); i++)
    {
        std::cout << "Destroy work on topic " << i->first << std::endl;
        delete i->second;
    }
    this->producer->flush(5000);
    delete this->producer;
}

Recorder::Worker *Recorder::addWorker(
    const std::string &topicName,
    Filter *filter,
    const time_t interval)
{
    Recorder::Worker *worker = new Recorder::Worker(
        this->producer,
        topicName,
        filter,
        interval);
    this->workers.insert({topicName, worker});
    return worker;
}

Recorder::Worker::Worker(
    RdKafka::Producer *handler,
    const std::string &topicName,
    Filter *filter,
    const time_t interval)
{
    std::string errMsg;
    RdKafka::Topic *topic = RdKafka::Topic::create(handler, topicName, NULL, errMsg);
    this->handler = handler;
    this->topic = topic;
    this->topicName = topicName;
    this->filter = filter;
    this->interval = interval;
    this->stopFlag = false;
    // this->job = std::thread(&Recorder::Worker::_sendMessage, this);
    this->_sendMessage();
}

Recorder::Worker *Recorder::getWorker(const std::string &topicName) const
{
    return this->workers.at(topicName);
}

Recorder::Worker::~Worker()
{
    // Gracefully terminate program
    this->stopFlag = true;
    // this->job.join();
    delete this->topic;
}
