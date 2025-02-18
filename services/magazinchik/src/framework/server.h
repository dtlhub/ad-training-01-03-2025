//
// Created by Андрей Шпак on 05.02.2025.
//

#ifndef MAGAZINCHIK_SERVER_H
#define MAGAZINCHIK_SERVER_H

namespace framework {
    void handle_client(int client_socket);
    void run(int port);
}

#endif //MAGAZINCHIK_SERVER_H
