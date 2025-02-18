#include "auth.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <string>

namespace auth {
    Session sessions[MAX_SESSIONS] = {};

    void generate_token(char* output_hash, size_t output_size) {
        if (output_size < 65) {
            fprintf(stderr, "Output buffer too small\n");
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 61);

        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        for (int i = 0; i < 64; i++) {
            output_hash[i] = charset[dis(gen)];
        }
        output_hash[64] = '\0';
    }

    void save_session_to_file(const Session& session) {
        std::ofstream out(SESSIONS_FILE, std::ios::app);
        if (out.is_open()) {
            out << session.username << ":" << session.session_token << "\n";
            out.close();
        } else {
            std::cerr << "Failed to open sessions file for writing." << std::endl;
        }
    }

    const char* login(const char* username) {
        static char session_token[65];

        generate_token(session_token, sizeof(session_token));
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (sessions[i].username[0] == '\0') {
                strncpy(sessions[i].username, username, sizeof(sessions[i].username) - 1);
                sessions[i].username[sizeof(sessions[i].username) - 1] = '\0';

                strncpy(sessions[i].session_token, session_token, sizeof(sessions[i].session_token)-1);
                sessions[i].session_token[sizeof(sessions[i].session_token) - 1] = '\0';

                save_session_to_file(sessions[i]);

                std::cout << "Session saved at index " << i << " for username: " << username << std::endl;
                return session_token;
            }
        }
        return nullptr;
    }

    int is_authorized(const char* token) {
        std::ifstream in(SESSIONS_FILE);
        if (!in.is_open()) {
            std::cerr << "Failed to open sessions file for reading." << std::endl;
            return 0;
        }

        std::string line;
        while (std::getline(in, line)) {
            size_t delimiter_pos = line.find(':');
            if (delimiter_pos == std::string::npos) {
                continue;
            }

            std::string file_token = line.substr(delimiter_pos + 1);

            if (file_token == token) {
                in.close();
                return 1;
            }
        }

        in.close();
        std::cout << "Token not found" << std::endl;
        return 0;
    }

    std::string find_username_by_session(std::string& token) {
        std::ifstream in(SESSIONS_FILE);
        if (!in.is_open()) {
            std::cerr << "Failed to open sessions file for reading." << std::endl;
            return "";
        }

        std::string line;
        while (std::getline(in, line)) {
            size_t delimiter_pos = line.find(':');
            if (delimiter_pos == std::string::npos) {
                continue;
            }

            std::string file_username = line.substr(0, delimiter_pos);
            std::string file_token = line.substr(delimiter_pos + 1);
            file_token.erase(file_token.find_last_not_of("\r\n\0") + 1);

            token.erase(token.find_last_not_of("\r\n\0") + 1);

            if (file_token == token) {
                in.close();
                return file_username;
            }
        }

        in.close();
        return "";
    }
}