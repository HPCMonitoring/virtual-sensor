#ifndef __UTILS_H__
#define __UTILS_H__

#include "main.h"

namespace utils
{
    std::string strReplace(
        const std::string &source,
        const std::string &target,
        const std::string &replacement)
    {
        std::string copy = source;
        size_t pos = copy.find(target);
        while (pos != std::string::npos)
        {
            copy.replace(pos, target.length(), replacement);
            pos = copy.find(target, pos + replacement.length());
        }
        return copy;
    }
}

#endif