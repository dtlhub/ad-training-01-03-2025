#ifndef MAGAZINCHIK_SERVER_H
#define MAGAZINCHIK_SERVER_H

#include <iostream>

namespace framework {
    void handle_client(int client_socket);
    void run(int port);
}

#endif //MAGAZINCHIK_SERVER_H