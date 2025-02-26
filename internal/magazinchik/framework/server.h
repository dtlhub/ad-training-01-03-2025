#ifndef MAGAZINCHIK_SERVER_H
#define MAGAZINCHIK_SERVER_H

#include <iostream>

namespace framework {
    void handle_client(int client_socket);
    void run(int port, size_t max_threads = 4);
}

#endif //MAGAZINCHIK_SERVER_H