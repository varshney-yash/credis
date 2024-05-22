// runner.h
#ifndef RUNNER_H
#define RUNNER_H

#include <sys/epoll.h>
#include <netinet/in.h>
#include <string>

class Runner {
public:
    void run_server(int argc, char* argv[]);
private:
    void accept_connection(int epoll_fd, int server_fd);
    void handle_client(int epoll_fd, int client_fd);
    std::string parse_request(const std::string& request);
    void send_response(int client_fd, const std::string& response);
    void close_connection(int epoll_fd, int client_fd);
};

#endif // RUNNER_H