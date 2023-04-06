#include "worker_repository.h"

WorkerRepository *WorkerRepository::_singleton = nullptr;
std::mutex WorkerRepository::_singletonMutex;

WorkerRepository::WorkerRepository() {
    // do nothing
}

void WorkerRepository::clearAll() {
    for (auto & it : this->producers) {
        delete it.second;
    }
    this->producers.clear();
}

void WorkerRepository::addWorker(std::string brokerUrl, MessageProducer::WorkerProp *prop) {
    auto iter = this->producers.find(brokerUrl);
    if (iter == this->producers.end()) {
        // TODO: replace hard code clientId here
        auto producer = new MessageProducer("sensor-manager-api", brokerUrl);
        this->producers.insert({brokerUrl, producer});
    }

    auto producer = this->producers.at(brokerUrl);
    producer->createWorker(prop);

}

WorkerRepository* WorkerRepository::getInstance() {
    std::lock_guard<std::mutex> lock(_singletonMutex);
    if (_singleton == nullptr)
    {
        _singleton = new WorkerRepository();
    }
    return _singleton;
}


