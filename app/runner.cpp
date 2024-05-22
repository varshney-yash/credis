#include "runner.h"
#include "config.h"
#include "resolve.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

void Runner::run_server(int argc, char* argv[]) {
    Environment::load();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(Environment::HOST.c_str());
    server_addr.sin_port = htons(Environment::PORT);

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 100);

    int epoll_fd = epoll_create1(0);
    epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    while (true) {
        epoll_event events[100];
        int event_count = epoll_wait(epoll_fd, events, 100, -1);
        for (int i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_fd) {
                accept_connection(epoll_fd, server_fd);
            } else {
                handle_client(epoll_fd, events[i].data.fd);
            }
        }
    }
}

void Runner::accept_connection(int epoll_fd, int server_fd) {
    int client_fd = accept(server_fd, nullptr, nullptr);
    epoll_event event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    std::cout << "Accepted connection from client " << client_fd << std::endl;
}

void Runner::handle_client(int epoll_fd, int client_fd) {
    char buffer[1024];
    int bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        close_connection(epoll_fd, client_fd);
        return;
    }
    std::string request(buffer, bytes_read);
    std::cout << "Received request: " << request << std::endl;
    std::string response = parse_request(request);
    std::cout << "Sending response: " << response << std::endl;
    send_response(client_fd, response);
}

std::string Runner::parse_request(const std::string& request) {
    std::string method = request.substr(0, request.find(' '));
    if (method == "SET") {
        return resolve_set(request);
    } else if (method == "GET") {
        return resolve_get(request);
    } else if (method == "PING") {
        return resolve_ping();
    } else {
        return "ERR unknown command";
    }
}

void Runner::send_response(int client_fd, const std::string& response) {
    std::string formatted_response = response + "\r\n";
    write(client_fd, formatted_response.c_str(), formatted_response.size());
}

void Runner::close_connection(int epoll_fd, int client_fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
    close(client_fd);
    std::cout << "Closed connection with client " << client_fd << std::endl;
}

// Main function to start the server
int main(int argc, char* argv[]) {
    Runner runner;
    runner.run_server(argc, argv);
    return 0;
}
