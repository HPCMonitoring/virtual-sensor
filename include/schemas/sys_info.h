#ifndef __VIRTUAL_SENSOR_SYS_INFO_H__
#define __VIRTUAL_SENSOR_SYS_INFO_H__

#include "schemas/json_serializable.h"
#include <boost/format.hpp>

class SysInfo : public JsonSerializable{
public:
    std::string kernelName;
    std::string kernelVersion;
    std::string arch;
    std::string hostname;
    std::string rootUser;
    virtual std::string toJson() const override;
};


#endif //VIRTUAL_SENSOR_SYS_INFO_H
