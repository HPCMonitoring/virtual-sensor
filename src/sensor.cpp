#include "sensor.h"

#ifdef __SENSOR_H__

Sensor::Sensor(const std::string &credentialStorePath)
{
    std::cout << "Load credential from " << credentialStorePath << std::endl;
}

Sensor::~Sensor()
{
    for (std::unordered_map<std::string, MessageProducer *>::iterator i = this->kafkaProducers.begin(); i != this->kafkaProducers.end(); i++)
    {
        std::cout << i->first << std::endl;
        delete i->second;
    }
}

MessageProducer *Sensor::addKafkaProducer(const std::string &brokerUrl, const std::string &clientId)
{
    MessageProducer *producer = new MessageProducer(clientId, brokerUrl);
    this->kafkaProducers.insert({brokerUrl, producer});
    return producer;
}

void Sensor::removeKafkaProducer(const std::string &brokerUrl)
{
    MessageProducer *producer = this->kafkaProducers.at(brokerUrl);
    this->kafkaProducers.erase(brokerUrl);
    delete producer;
}

MessageProducer *Sensor::getKafkaProducer(const std::string &brokerUrl)
{
    return this->kafkaProducers.at(brokerUrl);
}

void Sensor::connectToSensorManagerServer()
{
    std::cout << "Connecting to sensor manager server ..." << std::endl;
    sleep(500);
    std::cout << "Connected successfully !" << std::endl;
}

#endif