// resolve.h
#ifndef RESOLVE_H
#define RESOLVE_H

#include <string>

std::string resolve_set(const std::string& data);
std::string resolve_get(const std::string& data);
std::string resolve_delete(const std::string& data);
std::string resolve_ping();
void save_snapshot(const std::string& filename);
void load_snapshot(const std::string& filename);

#endif // RESOLVE_H