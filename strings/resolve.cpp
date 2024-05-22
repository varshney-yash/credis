#include "resolve.h"
#include <unordered_map>

static std::unordered_map<std::string, std::string> key_dict;

std::string resolve_set(const std::string& data) {
    size_t key_start = data.find(' ') + 1;
    size_t key_end = data.find(' ', key_start);
    std::string key = data.substr(key_start, key_end - key_start);
    std::string value = data.substr(key_end + 1);
    key_dict[key] = value;
    return "OK";
}

std::string resolve_get(const std::string& data) {
    size_t key_start = data.find(' ') + 1;
    size_t key_end = data.find("\r\n", key_start);
    std::string key = data.substr(key_start, key_end - key_start);
    if (key_dict.find(key) != key_dict.end()) {
        return key_dict[key];
    } else {
        return "NULL";
    }
}

std::string resolve_ping() {
    return "PONG";
}
