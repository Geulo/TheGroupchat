#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int chat_client_run(const char *server_ip) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    printf("Connecting to server %s:8080...\n", server_ip);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected!\n");

    // TEMP TEST: receive welcome message
    char buf[1024];
    ssize_t n = recv(sock, buf, sizeof(buf)-1, 0);
    if (n > 0) {
        buf[n] = '\0';
        printf("Server: %s", buf);
    }

    close(sock);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2){printf("Usage: %s <server_ip", argv[0]); return 1;}

    return chat_client_run(argv[1]);
}