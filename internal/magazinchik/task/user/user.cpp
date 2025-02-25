#include "user.h"
#include "../authorization/auth.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

namespace user {
    bool add_user(char* username, char* password) {
        std::ofstream out(USER_FILE, std::ios::app);

        if (!out.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for writing" << USER_FILE << std::endl;
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
        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for reading" << USER_FILE << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string fileUsername, filePassword;
            int balance = 0;

            // Читаем данные с разделением на части
            if (std::getline(iss, fileUsername, ':') &&
                std::getline(iss, filePassword, ':') &&
                iss >> balance) {
                if (fileUsername == username && filePassword == password) {
                    return true;
                }
            } else {
                std::cerr << "[ERROR] Failed to parse line: " << line << std::endl;
            }
        }
        return false;
    }

    User find_user_by_username(char* username) {
        User u = {};
        std::ifstream in(USER_FILE);
        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for reading" << USER_FILE << std::endl;
            return u;
        }

        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string fileUsername, filePassword;
            int balance = 0;

            // Читаем данные с разделением на части
            if (std::getline(iss, fileUsername, ':') &&
                std::getline(iss, filePassword, ':') &&
                iss >> balance) {
                if (fileUsername == username) {
                    strncpy(u.username, fileUsername.c_str(), sizeof(u.username) - 1);
                    u.username[sizeof(u.username) - 1] = '\0';
                    strncpy(u.password, filePassword.c_str(), sizeof(u.password) - 1);
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

    bool changePassword(char* username, char* new_password) {
        std::ifstream in(USER_FILE);
        if (!in.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file for reading" << USER_FILE << std::endl;
            return false;
        }

        std::vector<std::string> lines;
        std::string line;
        bool found = false;

        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string fileUsername, filePassword;
            int balance = 0;

            // Читаем данные с разделением на части
            if (std::getline(iss, fileUsername, ':') &&
                std::getline(iss, filePassword, ':') &&
                iss >> balance) {
                if (fileUsername == username) {
                    found = true;
                    lines.push_back(fileUsername + ":" + new_password + ":" + std::to_string(balance));
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