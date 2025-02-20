#ifndef MAGAZINCHIK_AUTH_H
#define MAGAZINCHIK_AUTH_H

#define MAX_SESSIONS 1000000

#include <iostream>

#define SESSIONS_FILE "/tmp/sessions.txt"

namespace auth {
    struct Session {
        char username[32];
        char session_token[65];
    };

    extern Session sessions[MAX_SESSIONS];

    void clean_sessions();

    void generate_token(char *buffer, size_t length);
    const char* login(const char *username);
    int is_authorized(const char *token);
    std::string find_username_by_session(std::string& token);
}

#endif //MAGAZINCHIK_AUTH_H