#ifndef __VIRTUAL_SENSOR_PLAINJSONSTR_H__
#define __VIRTUAL_SENSOR_PLAINJSONSTR_H__

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
