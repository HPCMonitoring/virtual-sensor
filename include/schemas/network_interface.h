#ifndef __NETWORK_INTERFACE_H__
#define __NETWORK_INTERFACE_H__

#include "main.h"

struct InterfaceData
{
    std::string name;
};

class NetworkInterface
{
public:
    class ReceiveData;
    class TransmitData;

private:
    std::string name;
    ReceiveData *receive;
    TransmitData *transmit;

public:
    NetworkInterface(const std::string& dataline);
    std::string getName() const;
    ReceiveData* getReceiveData() const;
    TransmitData* getTransmitData() const;
    ~NetworkInterface();

public:
    class ReceiveData
    {
    public:
        std::string bytes;
        std::string packets;
        std::string errors;
        std::string dropped;
        std::string fifo;
        std::string frame;
        std::string compressed;
        std::string multicast;
    };

    class TransmitData
    {
    public:
        std::string bytes;
        std::string packets;
        std::string errors;
        std::string dropped;
        std::string fifo;
        std::string colls;
        std::string carrier;
        std::string compressed;
    };
};

#endif