#include "parser.h"
#include <sstream>
#include <iostream>

namespace parser {

    void parse_cookies(const std::string &cookie_str, std::unordered_map<std::string, std::string> &cookies) {
        std::istringstream cookie_stream(cookie_str);
        std::string cookie_pair;
        while (std::getline(cookie_stream, cookie_pair, ';')) {
            size_t delim_pos = cookie_pair.find('=');
            if (delim_pos != std::string::npos) {
                std::string key = cookie_pair.substr(0, delim_pos);
                std::string value = cookie_pair.substr(delim_pos + 1);
                key.erase(0, key.find_first_not_of(' '));
                key.erase(key.find_last_not_of(' ') + 1);
                value.erase(0, value.find_first_not_of(' '));
                value.erase(value.find_last_not_of(' ') + 1);
                cookies[key] = value;
            }
        }
    }

    Request parse_request(const std::string &raw_request) {
        Request req;
        std::istringstream stream(raw_request);
        std::string line;

        if (std::getline(stream, line)) {
            std::istringstream first_line(line);
            first_line >> req.method >> req.path;

            size_t q_pos = req.path.find('?');
            if (q_pos != std::string::npos) {
                req.query = req.path.substr(q_pos + 1);
                req.path = req.path.substr(0, q_pos);
            }
        }

        while (std::getline(stream, line) && line != "\r") {
            size_t delim_pos = line.find(": ");
            if (delim_pos != std::string::npos) {
                std::string key = line.substr(0, delim_pos);
                std::string value = line.substr(delim_pos + 2);
                req.headers[key] = value;

                if (key == "Cookie") {
                    parse_cookies(value, req.cookies);
                }
            }
        }

        if (req.headers.count("Content-Length")) {
            int content_length = std::stoi(req.headers["Content-Length"]);
            req.body.resize(content_length);
            stream.read(&req.body[0], content_length);
        }

        return req;
    }

    std::unordered_map<std::string, std::string> parse_post_body(const std::string &body) {
        std::unordered_map<std::string, std::string> params;
        std::istringstream body_stream(body);
        std::string pair;
        while (std::getline(body_stream, pair, '&')) {
            size_t delim_pos = pair.find('=');
            if (delim_pos != std::string::npos) {
                std::string key = pair.substr(0, delim_pos);
                std::string value = pair.substr(delim_pos + 1);
                params[key] = value;
            }
        }
        return params;
    }
}