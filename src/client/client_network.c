#include "client_network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int connect_to_server(const char *server_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return -1;
    }

    printf("Connecting to %s:%d...\n", server_ip, port);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    printf("Connected!\n");
    return sock;
}

int send_nickname(int sock, const char *nickname) {
    if (send(sock, nickname, strlen(nickname), 0) < 0) {
        perror("send nickname");
        return -1;
    }
    if (send(sock, "\n", 1, 0) < 0) {
        perror("send newline");
        return -1;
    }
    return 0;
}