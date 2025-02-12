//
// Created by Андрей Шпак on 06.02.2025.
//

#ifndef MAGAZINCHIK_USER_H
#define MAGAZINCHIK_USER_H


#define USER_FILE "/tmp/users.txt"

#include <iostream>

namespace user{
    struct User{
        char username[32];
        char password[32];
        int balance = 100;
    };

    bool add_user(char* username , char* password);
    bool checkUser(char* username , char* password);

    User find_user_by_session(std::string& session_token);
}

#endif //MAGAZINCHIK_USER_H
