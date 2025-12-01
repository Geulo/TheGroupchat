#include "server/network.h"
#include <sys/socket.h>
#include <stdio.h>

int create_socket(int *server_fd) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd < 0) {
        perror("ERROR CODE: function create_socket");
        return -1;
    }
    return 0;
}

int set_socket_reuse(int *socket_fd) {
    int opt = 1;
    if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("ERROR CODE: function set_socket_reuse");
        return -1;
    }
    return 0;
}

int bind_socket(int *socket_fd, struct sockaddr_in *addr) {
    if (bind(*socket_fd, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
        perror("ERROR CODE: function bind_socket");
        return -1;
    }
    return 0;
}

int start_listening(int *socket_fd, int backlog) {
    if (listen(*socket_fd, backlog) < 0) {
        perror("ERROR CODE: function start_listening");
        return -1;
    }
    return 0;
}
