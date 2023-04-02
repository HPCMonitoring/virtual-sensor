#include "utils.h"

#ifdef __UTILS_H__
namespace myutil
{
    std::string strReplace(std::string source, std::string target, std::string replacement)
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