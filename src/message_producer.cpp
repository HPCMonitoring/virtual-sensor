#include "message_producer.h"
#include "exceptions.h"
#include "repository.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

MessageProducer::Worker *MessageProducer::createWorker(MessageProducer::WorkerProp *prop)
{
    MessageProducer::Worker *worker = new MessageProducer::Worker(producer, prop);
    this->workers.insert({prop->topicName, worker});
    return worker;
}

MessageProducer::Worker::Worker(RdKafka::Producer *handler, WorkerProp *prop) {
    std::string errMsg;
    this->prop = prop;
    RdKafka::Topic *topic = RdKafka::Topic::create(handler, this->prop->topicName, NULL, errMsg);
    this->topic = topic;
    this->handler = handler;
    this->job = std::thread(&MessageProducer::Worker::_sendMessage, this);
}

void MessageProducer::removeWorker(const std::string &topicName)
{
    MessageProducer::Worker *worker = this->workers.at(topicName);
    delete worker;
    this->workers.erase(topicName);
}

MessageProducer::Worker *MessageProducer::getWorker(const std::string &topicName) const
{
    return this->workers.at(topicName);
}

MessageProducer::WorkerProp *MessageProducer::Worker::getProp() {
    return this->prop;
}

void MessageProducer::Worker::_sendMessage() {
    Repository &r = Repository::getInstance();

    while (stopFlag == false)
    {
        try {
            json stats = json::parse(r.getData(*this->prop->filter)).get<std::vector<json>>();
            for (auto stat : stats) {
                const std::string message = stat.dump();
                this->handler->produce(this->topic,
                                       RdKafka::Topic::PARTITION_UA,
                                       RdKafka::Producer::RK_MSG_COPY,
                                       const_cast<char *>(message.c_str()),
                                       message.size(),
                                       NULL,  // Key
                                       0,     // Key length
                                       NULL); // Opaque value
            }
        } catch (std::exception &exception) {
            // TODO: SPD log error here
            std::cout << exception.what();
        }
        sleep(this->prop->interval);
    }

}
MessageProducer::Worker::~Worker()
{
    // Gracefully terminate program
    this->stopFlag = true;
    this->job.join();
    delete this->topic;
    delete this->prop;
}

MessageProducer::WorkerProp::WorkerProp(const std::string &topicName, Filter *filter, const time_t interval) :
        topicName(topicName), filter(filter), interval(interval) {
    // do nothing
}

MessageProducer::WorkerProp::~WorkerProp() {
    delete this->filter;
}
