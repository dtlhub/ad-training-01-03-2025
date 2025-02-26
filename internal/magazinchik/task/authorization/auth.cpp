#include "auth.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

namespace auth {
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

    const char* login(const char* username) {
        static char session_token[65];

        generate_token(session_token, sizeof(session_token));

        fs::create_directories(USERS_DIR);
        fs::create_directories(SESSIONS_DIR);
        fs::create_directories(ORDERS_DIR);

        std::ofstream session_file(SESSIONS_DIR + session_token);
        if (session_file.is_open()) {
            session_file << username;
            session_file.close();
            return session_token;
        } else {
            std::cerr << "Failed to save session." << std::endl;
            return nullptr;
        }
    }

    int is_authorized(const char* token) {
        std::ifstream session_file(SESSIONS_DIR + token);
        return session_file.is_open();
    }

    std::string find_username_by_session(const std::string& token) {
        std::ifstream session_file(SESSIONS_DIR + token);
        if (session_file.is_open()) {
            std::string username;
            session_file >> username;
            return username;
        }
        return "";
    }
}