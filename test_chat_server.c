#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int chat_server_run(void) {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }

    printf("Client connected!\n");

    char buf[1024];
    while (1) {
        ssize_t n = recv(client_fd, buf, sizeof(buf)-1, 0);
        if (n <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        buf[n] = '\0';
        printf("Client says: %s", buf);
    }

    close(client_fd);
    close(server_fd);

    return 0;
}
