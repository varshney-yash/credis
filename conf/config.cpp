// config.cpp
#include "config.h"
#include <cstdlib>

std::string Environment::HOST = "127.0.0.1";
int Environment::PORT = 8765;

void Environment::load() {
    if (const char* env_host = std::getenv("HOST")) {
        HOST = env_host;
    }
    if (const char* env_port = std::getenv("PORT")) {
        PORT = std::atoi(env_port);
    }
}