#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>

namespace parser {
    struct Request {
        std::string method;
        std::string path;
        std::string query;
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> cookies;
        std::string body;
        std::string raw_request;
    };

    Request parse_request(const std::string &raw_request);
    std::unordered_map<std::string, std::string> parse_post_body(const std::string &body);
    void parse_cookies(const std::string &cookie_str, std::unordered_map<std::string, std::string> &cookies);
    std::unordered_map<std::string, std::string> parse_query_params(const std::string &query);
}

#endif // PARSER_H