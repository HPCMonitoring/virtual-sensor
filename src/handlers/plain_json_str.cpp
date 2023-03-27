#include "handlers/plain_json_str.h"

PlainJsonStr::PlainJsonStr(std::string val) {
    this->val = val;
}

std::string PlainJsonStr::toJson() const {
    return this->val;
}
