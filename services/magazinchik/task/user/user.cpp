#include "user.h"
#include "../authorization/auth.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace user {
    bool add_user(char* username, char* password) {
        std::ofstream out(USER_FILE, std::ios::app);

        if (!out.is_open()) {
            std::cout << "[ERROR] Couldn't open file for writing";
            return false;
        }

        User newUser;
        strcpy(newUser.username, username);
        strcpy(newUser.password, password);

        out << newUser.username << ":" << newUser.password << ":" << newUser.balance << "\n";
        out.close();
        return true;
    }

    bool checkUser(char* username, char* password) {
        std::ifstream in(USER_FILE);
        if (!in) {
            std::cerr << "[ERROR] Couldn't open file" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(in, line)) {
            char fileUsername[32], filePassword[32];
            int balance;

            if (sscanf(line.c_str(), "%31[^:]:%31[^:]:%d", fileUsername, filePassword, &balance) == 3) {
                if (strcmp(fileUsername, username) == 0 && strcmp(filePassword, password) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    User find_user_by_username(char* username) {
        User u = {};
        std::ifstream in(USER_FILE);
        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for reading" << std::endl;
            return u;
        }

        std::string line;
        while (std::getline(in, line)) {
            char fileUsername[32] = {};
            char filePassword[32] = {};
            int balance = 0;

            if (sscanf(line.c_str(), "%31[^:]:%31[^:]:%d", fileUsername, filePassword, &balance) == 3) {
                if (strcmp(fileUsername, username) == 0) {
                    strncpy(u.username, fileUsername, sizeof(u.username) - 1);
                    u.username[sizeof(u.username) - 1] = '\0';
                    strncpy(u.password, filePassword, sizeof(u.password) - 1);
                    u.password[sizeof(u.password) - 1] = '\0';
                    u.balance = balance;
                    in.close();
                    return u;
                }
            } else {
                std::cerr << "[ERROR] Failed to parse line: " << line << std::endl;
            }
        }
        in.close();
        return u;
    }

    bool changePassword( char* username,  char* new_password) {
        std::ifstream in(USER_FILE);
        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for reading" << std::endl;
            return false;
        }

        std::vector<std::string> lines;
        std::string line;
        bool found = false;

        while (std::getline(in, line)) {
            char fileUsername[32], filePassword[32];
            int balance;
            if (sscanf(line.c_str(), "%31[^:]:%31[^:]:%d", fileUsername, filePassword, &balance) == 3) {
                if (strcmp(fileUsername, username) == 0) {
                    found = true;
                    lines.push_back(std::string(fileUsername) + ":" + new_password + ":" + std::to_string(balance));
                } else {
                    lines.push_back(line);
                }
            }
        }
        in.close();

        if (!found) {
            std::cerr << "[ERROR] User not found" << std::endl;
            return false;
        }

        std::ofstream out(USER_FILE, std::ios::trunc);
        if (!out.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for writing" << std::endl;
            return false;
        }

        for (const auto& l : lines) {
            out << l << "\n";
        }
        out.close();
        return true;
    }
}