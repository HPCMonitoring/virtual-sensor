#include "schemas/network_interface.h"

NetworkInterface::NetworkInterface(const std::string &dataline)
{
    std::istringstream iss(dataline);
    this->receive = new ReceiveData();
    this->transmit = new TransmitData();
    iss >> this->name;
    this->name.pop_back(); // Remove ":" character
    iss >> this->receive->bytes >> this->receive->packets >> this->receive->errors >> this->receive->dropped >> this->receive->fifo >> this->receive->frame >> this->receive->compressed >> this->receive->multicast >> this->transmit->bytes >> this->transmit->packets >> this->transmit->errors >> this->transmit->dropped >> this->transmit->fifo >> this->transmit->colls >> this->transmit->carrier >> this->transmit->compressed;
}

std::string NetworkInterface::getName() const
{
    return this->name;
}

NetworkInterface::ReceiveData *NetworkInterface::getReceiveData() const
{
    return this->receive;
}
NetworkInterface::TransmitData *NetworkInterface::getTransmitData() const
{
    return this->transmit;
}

NetworkInterface::~NetworkInterface()
{
    delete this->receive;
    delete this->transmit;
}