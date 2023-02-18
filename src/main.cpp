// // #include "sensor.h"
// // #include "message_producer.h"

// #include <iostream>
// #include <librdkafka/rdkafkacpp.h>

// int main(int argc, char *argv[])
// {
//     // std::string credentialPath;

//     // if (argc == 1)
//     // {
//     //     credentialPath = "/usr/share/bkvsensor";
//     // }
//     // else
//     // {
//     //     credentialPath = std::string(argv[1]);
//     // }

//     // Sensor sensor = Sensor(credentialPath);
//     // sensor.addKafkaProducer("localhost:9092", "bkvsensor");

//     // MessageProducer producer = MessageProducer("bkvsensor", "localhost:9092");

//     std::string errstr;
//     RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

//     conf->set("client.id", "bkvsensor", errstr);
//     conf->set("bootstrap.servers", "localhost:9092", errstr);

//     RdKafka::Producer *kafkaProducer = RdKafka::Producer::create(conf, errstr);

//     if (!kafkaProducer)
//     {
//         throw std::runtime_error(errstr);
//     }

//     kafkaProducer->flush(5000);

//     delete kafkaProducer;
//     delete conf;

//     return 0;
// }

#include <iostream>
#include <librdkafka/rdkafkacpp.h>

int main(int argc, char *argv[])
{
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    
    conf->set("client.id", "bkvsensor", errstr);
    conf->set("bootstrap.servers", "localhost:9092", errstr);

    RdKafka::Producer* kafkaProducer = RdKafka::Producer::create(conf, errstr);
    if (!kafkaProducer)
    {
        throw std::runtime_error(errstr);
    }
    kafkaProducer->flush(5000);
    delete conf;
    delete kafkaProducer;
    return 0;
}
