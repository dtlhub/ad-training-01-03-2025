#ifndef MAGAZINCHIK_AUTH_H
#define MAGAZINCHIK_AUTH_H

#include <iostream>
#include <string>

namespace auth {
    const std::string USERS_DIR = "/tmp/users/";
    const std::string SESSIONS_DIR = "/tmp/sessions/";
    const std::string ORDERS_DIR = "/tmp/orders/";

    void generate_token(char* buffer, size_t length);
    const char* login(const char* username);
    int is_authorized(const char* token);
    std::string find_username_by_session(const std::string& token);
}

#endif // MAGAZINCHIK_AUTH_H