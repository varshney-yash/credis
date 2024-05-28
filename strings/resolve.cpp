#include "resolve.h"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>

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
    std::string pattern = data.substr(key_start);
    
    if (pattern == "*") {
        key_dict.clear();
        return "OK";
    }

    std::string regex_pattern = "^" + std::regex_replace(pattern, std::regex("\\*"), ".*") + "$";
    std::regex re(regex_pattern);
    std::vector<std::string> keys_to_delete;

    for (const auto& pair : key_dict) {
        if (std::regex_match(pair.first, re)) {
            keys_to_delete.push_back(pair.first);
        }
    }

    for (const auto& key : keys_to_delete) {
        key_dict.erase(key);
    }

    return "OK";
}

std::string resolve_search(const std::string& data) {
    size_t pattern_start = data.find(' ') + 1;
    std::string pattern = data.substr(pattern_start);
    std::string regex_pattern = "^" + std::regex_replace(pattern, std::regex("\\*"), ".*") + "$";
    std::regex re(regex_pattern);

    std::ostringstream oss;
    for (const auto& pair : key_dict) {
        if (std::regex_match(pair.first, re)) {
            oss << pair.first << "\n";
        }
    }
    return oss.str();
}

std::string resolve_scan(const std::string& data) {
    size_t cursor_start = data.find(' ') + 1;
    size_t cursor_end = data.find(' ', cursor_start);
    std::string cursor_str = data.substr(cursor_start, cursor_end - cursor_start);
    size_t cursor = std::stoul(cursor_str);
    
    size_t count_start = data.find(' ', cursor_end) + 1;
    std::string count_str = data.substr(count_start);
    size_t count = std::stoul(count_str);
    
    std::ostringstream oss;
    auto it = key_dict.begin();
    std::advance(it, cursor);

    for (size_t i = 0; i < count && it != key_dict.end(); ++i, ++it) {
        oss << it->first << " : " << it->second << "\n";
    }

    return oss.str();
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