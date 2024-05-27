#include "runner.h"
#include "resolve.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <sstream> 
#include <string>  

void test_resolve_set_get() {
    std::string set_request = "SET key1 value1";
    std::string get_request = "GET key1";
    assert(resolve_set(set_request) == "OK");
    assert(resolve_get(get_request) == "value1");
    std::cout << "test_resolve_set_get passed." << std::endl;
}

void test_resolve_delete() {
    std::string set_request = "SET key2 value2";
    std::string delete_request = "DELETE key2";
    std::string get_request = "GET key2";
    assert(resolve_set(set_request) == "OK");
    assert(resolve_delete(delete_request) == "OK");
    assert(resolve_get(get_request) == "NULL");
    std::cout << "test_resolve_delete passed." << std::endl;
}

void test_resolve_ping() {
    assert(resolve_ping() == "PONG");
    std::cout << "test_resolve_ping passed." << std::endl;
}

void test_snapshot() {
    std::string set_request1 = "SET key3 value3";
    std::string set_request2 = "SET key4 value4";
    resolve_set(set_request1);
    resolve_set(set_request2);

    save_snapshot("test_snapshot.txt");

    resolve_delete("DELETE key3");
    resolve_delete("DELETE key4");

    load_snapshot("test_snapshot.txt");

    assert(resolve_get("GET key3") == "value3");
    assert(resolve_get("GET key4") == "value4");

    std::remove("test_snapshot.txt");

    std::cout << "test_snapshot passed." << std::endl;
}

std::vector<std::string> split_lines(const std::string& str) {
    std::vector<std::string> lines;
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}

void clear_database() {
    std::string get_all_request = "GET *";
    std::string all_keys_response = resolve_get(get_all_request);
    auto keys = split_lines(all_keys_response);
    for (const auto& key_value : keys) {
        std::string key = key_value.substr(0, key_value.find(" : "));
        resolve_delete("DELETE " + key);
    }
}

void test_get_all() {
    clear_database(); 

    std::string get_all_request = "GET *";
    std::string actual_response = resolve_get(get_all_request);
    std::string expected_response = "";

    assert(actual_response == expected_response);

    std::string set_request1 = "SET key5 value5";
    std::string set_request2 = "SET key6 value6";
    resolve_set(set_request1);
    resolve_set(set_request2);

    actual_response = resolve_get(get_all_request);
    expected_response = "key5 : value5\nkey6 : value6\n";

    auto actual_lines = split_lines(actual_response);
    auto expected_lines = split_lines(expected_response);
    std::sort(actual_lines.begin(), actual_lines.end());
    std::sort(expected_lines.begin(), expected_lines.end());

    assert(actual_lines == expected_lines);

    resolve_delete("DELETE key5");
    resolve_delete("DELETE key6");

    std::cout << "test_get_all passed." << std::endl;
}

void test_invalid_set() {
    std::string invalid_set_request = "SET * value";
    assert(resolve_set(invalid_set_request) == "ERROR: Invalid key\n");
    std::cout << "test_invalid_set passed." << std::endl;
}

void test_search() {
    clear_database(); 

    std::string set_request1 = "SET key7 value7";
    std::string set_request2 = "SET key8 value8";
    std::string set_request3 = "SET otherkey value9";
    resolve_set(set_request1);
    resolve_set(set_request2);
    resolve_set(set_request3);

    std::string search_request = "SEARCH key*";
    std::string actual_response = resolve_search(search_request);
    std::string expected_response = "key7\nkey8\n";

    auto actual_lines = split_lines(actual_response);
    auto expected_lines = split_lines(expected_response);
    std::sort(actual_lines.begin(), actual_lines.end());
    std::sort(expected_lines.begin(), expected_lines.end());

    assert(actual_lines == expected_lines);

    std::cout << "test_search passed." << std::endl;
}

void test_scan() {
    clear_database(); 

    std::string set_request1 = "SET key9 value9";
    std::string set_request2 = "SET key10 value10";
    resolve_set(set_request1);
    resolve_set(set_request2);

    std::string scan_request = "SCAN 0 2";
    std::string actual_response = resolve_scan(scan_request);
    std::string expected_response = "key9 : value9\nkey10 : value10\n";

    auto actual_lines = split_lines(actual_response);
    auto expected_lines = split_lines(expected_response);
    std::sort(actual_lines.begin(), actual_lines.end());
    std::sort(expected_lines.begin(), expected_lines.end());

    assert(actual_lines == expected_lines);

    std::cout << "test_scan passed." << std::endl;
}

int main() {
    test_resolve_set_get();
    test_resolve_delete();
    test_resolve_ping();
    test_snapshot();
    test_get_all();
    test_invalid_set();
    test_search();
    test_scan();
    std::cout << "All tests passed." << std::endl;
    return 0;
}