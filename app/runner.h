// runner.h
#ifndef RUNNER_H
#define RUNNER_H

#include <string>

class Runner {
public:
    static void run_server(int argc, char* argv[]);
    static void accept_connection(int epoll_fd, int server_fd);
    static void handle_connection(int epoll_fd, int client_fd);
    static std::string parse_request(const std::string& request);
    static void send_response(int client_fd, const std::string& response);
    static void close_connection(int epoll_fd, int client_fd);
};

#endif // RUNNER_H