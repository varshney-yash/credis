#include "runner.h"
#include "config.h"
#include "resolve.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>

void Runner::run_server(int argc, char* argv[]) {
    Environment::load();
    std::cout << "Starting server on " << Environment::HOST << ":" << Environment::PORT << std::endl;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(Environment::HOST.c_str());
    server_addr.sin_port = htons(Environment::PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 100) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on " << Environment::HOST << ":" << Environment::PORT << std::endl;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (true) {
        epoll_event events[100];
        int event_count = epoll_wait(epoll_fd, events, 100, -1);
        if (event_count == -1) {
            perror("epoll_wait");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_fd) {
                accept_connection(epoll_fd, server_fd);
            } else {
                handle_connection(epoll_fd, events[i].data.fd);
            }
        }
    }
}

void Runner::accept_connection(int epoll_fd, int server_fd) {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

    epoll_event event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl");
        close(client_fd);
    }
}

void Runner::handle_connection(int epoll_fd, int client_fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        std::string request(buffer);
        std::string response = parse_request(request);
        send_response(client_fd, response);
    } else {
        close_connection(epoll_fd, client_fd);
    }
}

std::string Runner::parse_request(const std::string& request) {
    std::string command;
    size_t end_pos = request.find("\r\n");
    if (end_pos != std::string::npos) {
        command = request.substr(0, end_pos);
    } else {
        command = request;
    }

    if (command.substr(0, 3) == "SET") {
        return resolve_set(command);
    } else if (command.substr(0, 3) == "GET") {
        return resolve_get(command);
    } else if (command.substr(0, 6) == "DELETE") {
        return resolve_delete(command);
    } else if (command.substr(0, 4) == "PING") {
        return resolve_ping();
    } else {
        return "ERROR: Unknown command\r\n";
    }
}

void Runner::send_response(int client_fd, const std::string& response) {
    std::string full_response = response + "\r\n";
    write(client_fd, full_response.c_str(), full_response.size());
}

void Runner::close_connection(int epoll_fd, int client_fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
    close(client_fd);
    std::cout << "Client disconnected" << std::endl;
}

int main(int argc, char* argv[]) {
    Runner::run_server(argc, argv);
    return 0;
}
