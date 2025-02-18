#include "router.h"
#include <iostream>
#include <sys/socket.h>

namespace router {

    std::vector<Route> routes;

    void add_route(const std::string &method, const std::string &path,
                   std::function<void(int, const parser::Request&)> handler) {
        routes.push_back({method, path, handler});
    }

    std::function<void(int, const parser::Request&)> find_route(const std::string &method, const std::string &path) {
        for (const auto &route : routes) {
            if (route.method == method && route.path == path) {
                return route.handler;
            }
        }
        return nullptr;
    }

    void handle_request(const std::string &method, const std::string &path, int client_socket, const parser::Request &req) {
        auto handler = find_route(method, path);
        if (handler) {
            handler(client_socket, req);
        } else {
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoute not found";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }
}