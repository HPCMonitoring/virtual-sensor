#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "main.h"
#include "schemas/process.h"
#include "clients/kafka_client.h"

class Sensor
{
private:
    // Hashmap kafka server url -> kafka producer
    std::unordered_map<std::string, KakfaClient*> kafkaProducers;
    std::string name;
    std::string cluster;
    std::string token;
    std::string managerUrl;
private:
    Sensor(const Sensor&) = delete;

public:
    Sensor(const std::string& credentialStorePath);
    KakfaClient* createMsgProducer(const std::string &clientId, const std::string &brokerUrl);
    void removeMsgProducer(const std::string &brokerUrl);
    KakfaClient* getMsgProducer(const std::string &brokerUrl);
    void connect();
    ~Sensor();
};

#endif