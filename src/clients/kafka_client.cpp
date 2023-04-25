#include "clients/kafka_client.h"

#include <utility>
#include "exceptions.h"
#include "repository/repository.h"
#include "sensor_logger.h"

KakfaClient::KakfaClient(const std::string &clientId, const std::string &brokerUrl)
{
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("client.id", clientId, errstr);
    conf->set("bootstrap.servers", brokerUrl, errstr);
    conf->set("linger.ms", "500", errstr);
    conf->set("acks", "0", errstr);

    RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(conf, errstr);

    if (!kafkaProducer)
        throw std::runtime_error(ERR_CREATE_PRODUCER + errstr);

    this->producer = std::shared_ptr<RdKafka::Producer>(kafkaProducer);
    delete conf;
}

KakfaClient::~KakfaClient()
{
    std::unordered_map<std::string, KakfaClient::Worker *>::iterator i;
    for (i = this->workers.begin(); i != this->workers.end(); i++)
    {
        i->second->stop();
    }
}

KakfaClient::Worker *KakfaClient::addWorker(KakfaClient::WorkerProp *prop)
{
    KakfaClient::Worker *worker = new KakfaClient::Worker(producer, prop);
    this->workers.insert({prop->topicName, worker});
    return worker;
}

KakfaClient::Worker::Worker(std::shared_ptr<RdKafka::Producer> handler, WorkerProp *prop)
{
    std::string errMsg;
    this->logger = SensorLogger::getInstance()->getLogger();

    SPDLOG_LOGGER_INFO(this->logger, "Create worker {} for topic {}", std::to_string(long(this)), prop->topicName);
    this->prop = prop;
    RdKafka::Topic *topic = RdKafka::Topic::create(handler.get(), this->prop->topicName, NULL, errMsg);
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
        try
        {
            SPDLOG_LOGGER_INFO(this->logger, "Worker {} begins getting data", std::to_string(long(this)));
            std::vector<std::string> records = r.getData(this->prop->filter);
            const size_t numOfRecords = records.size();
            SPDLOG_LOGGER_INFO(this->logger, "Worker {} found {} records", std::to_string(long(this)),  numOfRecords);
            for (size_t i = 0; i < numOfRecords; ++i)
            {
                std::stringstream ss;
                ss << records.at(i);
                ss.seekp(-1, std::ios_base::end);
                for (auto const &x : this->prop->headers)
                {
                    ss << ",\"" << x.first << "\":\"" << x.second << "\"";
                }
                ss << "}";

                std::string payload = ss.str();

                this->handler->produce(this->topic,
                                       RdKafka::Topic::PARTITION_UA,
                                       RdKafka::Producer::RK_MSG_COPY,
                                       const_cast<char *>(payload.c_str()),
                                       payload.size(),
                                       NULL,  // Key
                                       0,     // Key length
                                       NULL); // Opaque value
            }
        }
        catch (std::exception &exception)
        {
            // TODO: SPD log error here
            SPDLOG_LOGGER_ERROR(this->logger, "Fail to send message to kafka with error message: {}", exception.what());
        }
        std::this_thread::sleep_for(std::chrono::seconds(this->prop->interval));
    }

    // TODO: must review this delete statement to make code clean and understandable
    delete this;
}

void KakfaClient::Worker::stop()
{
    this->stopFlag = true;
}

KakfaClient::Worker::~Worker()
{
    SPDLOG_LOGGER_INFO(this->logger, "Destructor of worker {} for topic {} is called", std::to_string(long(this)), prop->topicName);

    // Gracefully terminate thread
    this->stopFlag.store(true, std::memory_order_release);
    delete this->topic;
    delete this->prop;
}

KakfaClient::WorkerProp::WorkerProp(std::string topicName, Filter *filter, const time_t interval, std::unordered_map<std::string, std::string> headers)
    : topicName(std::move(topicName)), filter(filter), interval(interval), headers(std::move(headers))
{
    // do nothing
}

KakfaClient::WorkerProp::~WorkerProp()
{
    delete this->filter;
}