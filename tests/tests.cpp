#include "runner.h"
#include "resolve.h"
#include <cassert>
#include <iostream>

void test_resolve_set_get() {
    std::string set_request = "SET key1 value1";
    std::string get_request = "GET key1";
    assert(resolve_set(set_request) == "OK");
    assert(resolve_get(get_request) == "value1");
    std::cout << "test_resolve_set_get passed." << std::endl;
}

void test_resolve_ping() {
    assert(resolve_ping() == "PONG");
    std::cout << "test_resolve_ping passed." << std::endl;
}

int main() {
    test_resolve_set_get();
    test_resolve_ping();
    std::cout << "All tests passed." << std::endl;
    return 0;
}
