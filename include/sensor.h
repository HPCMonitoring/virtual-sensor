#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "main.h"
#include "process.h"
#include "common.h"
#include "kafka_producer.h"

class Sensor
{
private:
    // Hashmap kafka server url -> kafka producer
    std::unordered_map<std::string, KafkaProducer*> kafkaProducers;
    std::string name;
    std::string cluster;
    std::string token;

public:
    Sensor(std::string credentialStorePath);
    void addKafkaProducer();
    void removeKafkaProducer();
    void getKafkaProducer();
    void connectToSensorManagerServer();
    ~Sensor();
};

#endif