#include "message_producer.h"
#include "exceptions.h"
#ifdef __MESSAGE_PRODUCER_H__

MessageProducer::MessageProducer(const std::string &clientId, const std::string &brokerUrl)
{
    std::string errstr;
    this->conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    conf->set("client.id", clientId, errstr);
    conf->set("bootstrap.servers", brokerUrl, errstr);

    RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(this->conf, errstr);

    if (!kafkaProducer)
    {
        throw std::runtime_error(ERR_CREATE_PRODUCER + errstr);
    }

    this->producer = kafkaProducer;
}

MessageProducer::~MessageProducer()
{
    std::cout << "DCM CLGT" << std::endl;
    this->producer->flush(100000);
    delete this->producer;
    delete this->conf;
}

#endif