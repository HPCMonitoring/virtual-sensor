#include "clients/kafka_client.h"
#include "exceptions.h"
#include "repository/repository.h"

KakfaClient::KakfaClient(const std::string &clientId, const std::string &brokerUrl)
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

KakfaClient::~KakfaClient()
{
    std::unordered_map<std::string, KakfaClient::Worker *>::iterator i;
    for (i = this->workers.begin(); i != this->workers.end(); i++)
    {
        i->second->stop();
    }
    this->producer->flush(5000);
    delete this->producer;
}

KakfaClient::Worker *KakfaClient::addWorker(KakfaClient::WorkerProp *prop)
{
    KakfaClient::Worker *worker = new KakfaClient::Worker(producer, prop);
    this->workers.insert({prop->topicName, worker});
    return worker;
}

KakfaClient::Worker::Worker(RdKafka::Producer *handler, WorkerProp *prop)
{
    std::string errMsg;
    this->prop = prop;
    RdKafka::Topic *topic = RdKafka::Topic::create(handler, this->prop->topicName, NULL, errMsg);
    this->topic = topic;
    this->handler = handler;
    this->stopFlag = false;
    this->job = std::thread(&KakfaClient::Worker::_sendMessage, this);
    this->job.detach();
}

KakfaClient::Worker *KakfaClient::getWorker(const std::string &topicName) const
{
    return this->workers.at(topicName);
}

void KakfaClient::Worker::_sendMessage()
{
    Repository &r = Repository::getInstance();

    while (!this->stopFlag.load(std::memory_order_acquire))
    {
        try {
            std::vector<std::string> records = r.getData(this->prop->filter);
            const size_t numOfRecords = records.size();
            for (size_t i = 0; i < numOfRecords; ++i) {
                this->handler->produce(this->topic,
                                       RdKafka::Topic::PARTITION_UA,
                                       RdKafka::Producer::RK_MSG_COPY,
                                       const_cast<char *>(records.at(i).c_str()),
                                       records.at(i).size(),
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

void KakfaClient::Worker::stop() {
    this->stopFlag = true;
}

KakfaClient::Worker::~Worker()
{
    // Gracefully terminate thread
    this->stopFlag.store(true, std::memory_order_release);
    delete this->topic;
    delete this->prop;
}

KakfaClient::WorkerProp::WorkerProp(const std::string &topicName, Filter *filter, const time_t interval) :
        topicName(topicName), filter(filter), interval(interval) {
    // do nothing
}

KakfaClient::WorkerProp::~WorkerProp() {
    delete this->filter;
}