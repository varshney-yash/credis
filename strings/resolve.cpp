#include "resolve.h"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>

static std::unordered_map<std::string, std::string> key_dict;

std::string resolve_ping() {
    return "PONG";
}

std::string resolve_set(const std::string& data) {
    if (data.find('*') != std::string::npos) {
        return "ERROR: Invalid key\n";
    }
    size_t key_start = data.find(' ') + 1;
    size_t key_end = data.find(' ', key_start);
    std::string key = data.substr(key_start, key_end - key_start);
    std::string value = data.substr(key_end + 1);
    key_dict[key] = value;
    return "OK";
}

std::string resolve_get(const std::string& data) {
    if (data == "GET *") {
        if (key_dict.empty()) {
            return "empty";
        }
        std::ostringstream oss;
        for (const auto& pair : key_dict) {
            oss << pair.first << " : " << pair.second << "\n";
        }
        return oss.str();
    }

    size_t key_start = data.find(' ') + 1;
    size_t key_end = data.find("\r\n", key_start);
    std::string key = data.substr(key_start, key_end - key_start);
    if (key_dict.find(key) != key_dict.end()) {
        return key_dict[key];
    } else {
        return "NULL";
    }
}

std::string resolve_delete(const std::string& data) {
    size_t key_start = data.find(' ') + 1;
    size_t key_end = data.find("\r\n", key_start);
    std::string key = data.substr(key_start, key_end - key_start);
    if (key_dict.erase(key)) {
        return "OK";
    } else {
        return "NULL";
    }
}


void save_snapshot(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cerr << "Error opening snapshot file for writing" << std::endl;
        return;
    }

    for (const auto& pair : key_dict) {
        ofs << pair.first << " " << pair.second << "\n";
    }

    ofs.close();
}

void load_snapshot(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Error opening snapshot file for reading" << std::endl;
        return;
    }

    key_dict.clear();
    std::string key, value;
    while (ifs >> key >> value) {
        key_dict[key] = value;
    }

    ifs.close();
}