#include "user.h"
#include "../authorization/auth.h"
#include <cstring>
#include <fstream>
#include <iostream>

namespace user {
    bool add_user(char* username, char* password) {
        std::ofstream out(USER_FILE, std::ios::app);

        if (!out.is_open()) {
            std::cout << "[ERROR] Couldn't open file for writing";
            return 0;
        }

        User newUser;
        strcpy(newUser.username, username);
        strcpy(newUser.password, password);

        out << newUser.username << ":" << newUser.password << ":" << newUser.balance << "\n";

        out.close();

        return 1;
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

    User find_user_by_session(std::string& session_token) {
        User foundUser = {};

        // Открытие файла с сессиями и поиск по сессии
        std::ifstream session_file("/tmp/sessions.txt");
        if (!session_file.is_open()) {
            std::cerr << "[ERROR] Couldn't open session file" << std::endl;
            return foundUser;
        }

        std::string line;
        std::string username;
        bool session_found = false;

        // Поиск в файле сессий
        while (std::getline(session_file, line)) {
            size_t delimiter_pos = line.find(':');
            if (delimiter_pos == std::string::npos) {
                continue;
            }

            std::string file_username = line.substr(0, delimiter_pos);
            std::string file_token = line.substr(delimiter_pos + 1);

            if (file_token == session_token) {
                username = file_username;
                session_found = true;
                break;
            }
        }

        session_file.close();

        if (!session_found) {
            std::cerr << "[ERROR] Session not found" << std::endl;
            return foundUser;
        }

        // Если сессия найдена, ищем пользователя в файле с пользователями
        std::ifstream user_file(USER_FILE);
        if (!user_file.is_open()) {
            std::cerr << "[ERROR] Couldn't open user file" << std::endl;
            return foundUser;
        }

        while (std::getline(user_file, line)) {
            char fileUsername[32], filePassword[32];
            int balance;

            // Разбираем строку из файла пользователя
            if (sscanf(line.c_str(), "%31[^:]:%31[^:]:%d", fileUsername, filePassword, &balance) == 3) {
                if (strcmp(fileUsername, username.c_str()) == 0) {
                    // Заполняем структуру пользователя
                    strncpy(foundUser.username, fileUsername, sizeof(foundUser.username) - 1);
                    strncpy(foundUser.password, filePassword, sizeof(foundUser.password) - 1);
                    foundUser.balance = balance;
                    break;
                }
            }
        }

        user_file.close();
        return foundUser;
    }


}