#ifndef VIRTUAL_SENSOR_PLAINJSONSTR_H
#define VIRTUAL_SENSOR_PLAINJSONSTR_H

#include "schema/json_serializable.h"
#include <string>

class PlainJsonStr : public JsonSerializable {
private:
    std::string val;
public:
    PlainJsonStr(std::string val);
    std::string toJson() const override;
};

#endif //VIRTUAL_SENSOR_PLAINJSONSTR_H
