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


    bool find_username_by_token(const std::string& session_token, char* out_username, size_t size) {
        std::ifstream session_file("/tmp/sessions.txt");
        if (!session_file.is_open()) {
            std::cerr << "[ERROR] Couldn't open session file" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(session_file, line)) {
            char file_username[32], file_token[65];
            if (sscanf(line.c_str(), "%31[^:]:%64s", file_username, file_token) == 2) {
                if (strcmp(file_token, session_token.c_str()) == 0) {
                    strncpy(out_username, file_username, size - 1);
                    out_username[size - 1] = '\0';
                    return true;
                }
            }
        }

        return false;
    }


}