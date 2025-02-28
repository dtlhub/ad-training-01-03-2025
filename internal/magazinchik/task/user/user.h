#ifndef MAGAZINCHIK_USER_H
#define MAGAZINCHIK_USER_H

#include <iostream>
#include <cstring>

// TODO: починить логин

namespace user {
    const std::string USERS_DIR = "/tmp/users/";

    struct User {
        char username[32];
        char password[32];
        int balance = 100;
    };

    bool add_user(const char* username, const char* password);
    bool checkUser(const char* username, const char* password);
    bool changePassword(const char* username, const char* new_password);
    User find_user_by_username(const char* username);
}

#endif // MAGAZINCHIK_USER_H