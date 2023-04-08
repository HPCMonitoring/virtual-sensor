#ifndef VIRTUAL_SENSOR_JSON_SERIALIZABLE_H
#define VIRTUAL_SENSOR_JSON_SERIALIZABLE_H

#include <string>

class JsonSerializable {
public:
    virtual std::string toJson() const = 0;
    virtual ~JsonSerializable() = default;
};


#endif //VIRTUAL_SENSOR_JSON_SERIALIZABLE_H
