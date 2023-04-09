#include "recorder.h"
#include "exceptions.h"
#include "repository.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Recorder::Recorder(const std::string &clientId, const std::string &brokerUrl)
{
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("client.id", clientId, errstr);
    conf->set("bootstrap.servers", brokerUrl, errstr);

    RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(conf, errstr);

    if (!kafkaProducer)
        throw std::runtime_error(ERR_CREATE_PRODUCER + errstr);

    this->producer = std::shared_ptr<RdKafka::Producer>(kafkaProducer);
    delete conf;
}

Recorder::~Recorder()
{
    std::unordered_map<std::string, Recorder::Worker *>::iterator i;
    for (i = this->workers.begin(); i != this->workers.end(); i++)
    {
        i->second->stop();
    }
    this->producer->flush(5000);
}

Recorder::Worker *Recorder::addWorker(Recorder::WorkerProp *prop)
{
    Recorder::Worker *worker = new Recorder::Worker(producer, prop);
    this->workers.insert({prop->topicName, worker});
    return worker;
}

Recorder::Worker::Worker(std::shared_ptr<RdKafka::Producer> handler, WorkerProp *prop)
{
    std::string errMsg;
    this->prop = prop;
    RdKafka::Topic *topic = RdKafka::Topic::create(handler.get(), this->prop->topicName, NULL, errMsg);
    this->topic = topic;
    this->handler = handler;
    this->stopFlag = false;
    this->job = std::thread(&Recorder::Worker::_sendMessage, this);
    this->job.detach();
}

Recorder::Worker *Recorder::getWorker(const std::string &topicName) const
{
    return this->workers.at(topicName);
}

void Recorder::Worker::_sendMessage()
{
    Repository &r = Repository::getInstance();

    while (!this->stopFlag.load(std::memory_order_acquire))
    {
        try {
            json stats = json::parse(r.getData(this->prop->filter)).get<std::vector<json>>();
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

    // TODO: must review this delete statement to make code clean and understandable
    delete this;
}

void Recorder::Worker::stop() {
    this->stopFlag = true;
}

Recorder::Worker::~Worker()
{
    // Gracefully terminate thread
    this->stopFlag.store(true, std::memory_order_release);
    delete this->topic;
    delete this->prop;
}

Recorder::WorkerProp::WorkerProp(const std::string &topicName, Filter *filter, const time_t interval) :
        topicName(topicName), filter(filter), interval(interval) {
    // do nothing
}

Recorder::WorkerProp::~WorkerProp() {
    delete this->filter;
}