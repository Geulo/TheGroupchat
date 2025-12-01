#include "client/client_network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int create_and_connect_socket(const char *server_ip) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);

    printf("Connecting to %s:8080...\n", server_ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    printf("Connected!\n");
    return sock;
}

int get_and_send_nickname(int sock) {
    char nickname[32];
    printf("Enter your nickname: ");
    fflush(stdout);
    
    if (!fgets(nickname, sizeof(nickname), stdin)) {
        return -1;
    }

    // Remove newline
    nickname[strcspn(nickname, "\n")] = 0;

    // Send nickname to server
    send(sock, nickname, strlen(nickname), 0);
    send(sock, "\n", 1, 0);
    
    return 0;
}
