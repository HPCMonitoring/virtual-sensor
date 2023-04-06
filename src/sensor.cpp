#include "sensor.h"

Sensor::Sensor(const std::string &managerServerUrl)
{
    this->managerUrl = managerServerUrl;
}

Sensor::~Sensor()
{
    for (std::unordered_map<std::string, Recorder *>::iterator i = this->kafkaProducers.begin(); i != this->kafkaProducers.end(); i++)
    {
        delete i->second;
    }
}

Recorder *Sensor::createMsgProducer(const std::string &clientId, const std::string &brokerUrl)
{
    Recorder *producer = new Recorder(clientId, brokerUrl);
    this->kafkaProducers.insert({brokerUrl, producer});
    return producer;
}

void Sensor::removeMsgProducer(const std::string &brokerUrl)
{
    Recorder *producer = this->kafkaProducers.at(brokerUrl);
    this->kafkaProducers.erase(brokerUrl);
    delete producer;
}

Recorder *Sensor::getMsgProducer(const std::string &brokerUrl)
{
    return this->kafkaProducers.at(brokerUrl);
}

void Sensor::connect()
{
    std::cout << "Connecting to sensor manager server: " << this->managerUrl << std::endl;
    sleep(1);
    std::cout << "Connected successfully !" << std::endl;
}
