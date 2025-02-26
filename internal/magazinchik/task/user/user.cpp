#include "user.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace user {
    bool add_user(const char* username, const char* password) {
        fs::create_directories(USERS_DIR);

        User u = {};

        std::ifstream user_file(USERS_DIR + username);
        if (user_file.is_open()) {
            std::cerr << "[ERROR] User already exists: " << username << std::endl;
            return false;
        }

        strcpy(u.username,username);
        strcpy(u.password , password);

        std::ofstream out(USERS_DIR + u.username);
        if (out.is_open()) {
            out << u.password << ":" << u.balance;
            out.close();
            return true;
        } else {
            std::cerr << "[ERROR] Couldn't create user file: " << username << std::endl;
            return false;
        }
    }

    bool checkUser(const char* username, const char* password) {
        std::ifstream user_file(USERS_DIR + username);
        if (!user_file.is_open()) {
            std::cerr << "[ERROR] User not found: " << username << std::endl;
            return false;
        }

        std::string file_password;
        int balance;

        std::getline(user_file, file_password, ':');
        user_file >> balance;

        file_password.erase(file_password.find_last_not_of("\r\n") + 1);

        if (file_password == password) {
            return true;
        } else {
            std::cerr << "[ERROR] Incorrect password for user: " << username << std::endl;
            return false;
        }
    }

    User find_user_by_username(const char* username) {
        User u = {};
        std::ifstream user_file(USERS_DIR + username);
        if (!user_file.is_open()) {
            std::cerr << "[ERROR] User not found: " << username << std::endl;
            return u;
        }

        std::string line;
        if (std::getline(user_file, line)) {
            size_t delimiter_pos = line.find(':');
            if (delimiter_pos != std::string::npos) {
                std::string file_password = line.substr(0, delimiter_pos);
                std::string balance_str = line.substr(delimiter_pos + 1);

                strncpy(u.username, username, sizeof(u.username) - 1);
                u.username[sizeof(u.username) - 1] = '\0';

                strncpy(u.password, file_password.c_str(), sizeof(u.password) - 1);
                u.password[sizeof(u.password) - 1] = '\0';

                u.balance = std::stoi(balance_str);
            } else {
                std::cerr << "[ERROR] Invalid format in user file: " << username << std::endl;
            }
        } else {
            std::cerr << "[ERROR] Failed to read user file: " << username << std::endl;
        }

        return u;
    }

    bool changePassword(const char* username, const char* new_password) {
        User u = find_user_by_username(username);
        if (u.username[0] == '\0') {
            std::cerr << "[ERROR] User not found: " << username << std::endl;
            return false;
        }

        std::ofstream out(USERS_DIR + username, std::ios::trunc);
        if (out.is_open()) {
            out << new_password << ":" << u.balance;
            out.close();
            return true;
        } else {
            std::cerr << "[ERROR] Couldn't update user file: " << username << std::endl;
            return false;
        }
    }
}