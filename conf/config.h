// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Environment {
public:
    static std::string HOST;
    static int PORT;
    static void load();
};

#endif // CONFIG_H