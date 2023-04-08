#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "main.h"
#include "process.h"
#include "recorder.h"

class Sensor
{
private:
    // Hashmap kafka server url -> kafka producer
    std::unordered_map<std::string, Recorder*> kafkaProducers;
    std::string name;
    std::string cluster;
    std::string token;
    std::string managerUrl;
private:
    Sensor(const Sensor&) = delete;

public:
    Sensor(const std::string& credentialStorePath);
    Recorder* createMsgProducer(const std::string &clientId, const std::string &brokerUrl);
    void removeMsgProducer(const std::string &brokerUrl);
    Recorder* getMsgProducer(const std::string &brokerUrl);
    void connect();
    ~Sensor();
};

#endif