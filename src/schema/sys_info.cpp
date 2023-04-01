#include <sstream>
#include <boost/format.hpp>
#include "schema/sys_info.h"

std::string SysInfo::toJson() const {
    auto format = boost::format("{\n"
                                "  \"kernelName\": \"%s\",\n"
                                "  \"kernelVersion\": \"%s\",\n"
                                "  \"arch\": \"%s\",\n"
                                "  \"hostname\": \"%s\",\n"
                                "  \"rootUser\": \"%s\"\n"
                                "}");
    return boost::str(
            format %
            this->kernelName %
            this->kernelVersion %
            this->arch %
            this->hostname %
            this->rootUser
            );
}
