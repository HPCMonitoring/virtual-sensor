#include <sstream>
#include "schema/sys_info.h"

std::string SysInfo::toJson() const {
    std::stringstream  ss;
    ss << "{"
       << "\"kernelName\":"     << "\"" << this->kernelName      << "\"" << ","
       << "\"kernelVersion\":"  << "\"" << this->kernelVersion   << "\"" << ","
       << "\"arch\":"           << "\"" << this->arch            << "\"" << ","
       << "\"hostname\":"       << "\"" << this->hostname        << "\"" << ","
       << "\"rootUser\":"       << "\"" << this->rootUser        << "\""
       << "}";
    return ss.str();
}
