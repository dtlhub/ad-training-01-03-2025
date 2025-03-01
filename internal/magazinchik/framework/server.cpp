#include "server.h"
#include "router/router.h"
#include "parser/parser.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <signal.h>
#include <sys/wait.h>

namespace framework {

    std::string read_socket_data(int client_socket) {
        char buffer[4096];
        std::string data;
        while (true) {
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) break;
            data.append(buffer, bytes_received);

            if (data.find("\r\n\r\n") != std::string::npos) {
                size_t content_length_pos = data.find("Content-Length: ");
                if (content_length_pos != std::string::npos) {
                    size_t content_length_end = data.find("\r\n", content_length_pos);
                    std::string content_length_str = data.substr(content_length_pos + 16, content_length_end - (content_length_pos + 16));
                    int content_length = std::stoi(content_length_str);
                    if (data.size() >= content_length + data.find("\r\n\r\n") + 4) {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
        return data;
    }

    void handle_client(int client_socket) {
        std::string raw_request = read_socket_data(client_socket);
        parser::Request req = parser::parse_request(raw_request);
        auto handler = router::find_route(req.method, req.path);

        if (handler) {
            handler(client_socket, req);
        } else {
            std::string response =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "<h1>404 Not Found</h1>";
            send(client_socket, response.c_str(), response.length(), 0);
        }

        close(client_socket);
    }

    void run(int port) {
        int server_fd;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Ошибка: не удалось создать сокет");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("Ошибка: не удалось настроить сокет");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("Ошибка: не удалось привязать сокет");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 10) < 0) {
            perror("Ошибка: не удалось запустить прослушивание");
            exit(EXIT_FAILURE);
        }

        std::cout << "[RUNNING] Сервер запущен на порту " << port << "\n";

        signal(SIGCHLD, SIG_IGN);

        while (true) {
            int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                perror("Ошибка: не удалось принять подключение");
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("Ошибка: не удалось создать процесс");
                close(new_socket);
            } else if (pid == 0) {
                close(server_fd);
                handle_client(new_socket);
                exit(0);
            } else {
                close(new_socket);
            }
        }

        close(server_fd);
    }
}