#ifndef __VIRTUAL_SENSOR_JSON_SERIALIZABLE_H__
#define __VIRTUAL_SENSOR_JSON_SERIALIZABLE_H__

#include "main.h"

class JsonSerializable {
public:
    virtual std::string toJson() const = 0;
    virtual ~JsonSerializable() = default;
};

#endif //VIRTUAL_SENSOR_JSON_SERIALIZABLE_H
