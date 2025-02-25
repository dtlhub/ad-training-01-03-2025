#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <functional>
#include <vector>
#include "../parser/parser.h"

namespace router {

    struct Route {
        std::string method;
        std::string path;
        std::function<void(int, const parser::Request&)> handler;
    };

    void add_route(const std::string &method, const std::string &path,
                   std::function<void(int, const parser::Request&)> handler);

    std::function<void(int, const parser::Request&)> find_route(const std::string &method, const std::string &path);

    void handle_request(const std::string &method, const std::string &path, int client_socket, const parser::Request &req);
}

#endif // ROUTER_H