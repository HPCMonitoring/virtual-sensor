#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "main.h"
#include "process.h"
#include "common.h"
#include "message_producer.h"

class Sensor
{
private:
    // Hashmap kafka server url -> kafka producer
    std::unordered_map<std::string, MessageProducer*> kafkaProducers;
    std::string name;
    std::string cluster;
    std::string token;
    std::string managerUrl;
private:
    Sensor(const Sensor&) = delete;

public:
    Sensor(const std::string& credentialStorePath);
    MessageProducer* createMsgProducer(const std::string &clientId, const std::string &brokerUrl);
    void removeMsgProducer(const std::string &brokerUrl);
    MessageProducer* getMsgProducer(const std::string &brokerUrl);
    void connect();
    ~Sensor();
};

#endif