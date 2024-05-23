#include "runner.h"
#include "resolve.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>

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

int main() {
    test_resolve_set_get();
    test_resolve_delete();
    test_resolve_ping();
    test_snapshot();
    std::cout << "All tests passed." << std::endl;
    return 0;
}
