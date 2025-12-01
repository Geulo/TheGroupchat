#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

int create_socket(int *server_fd);
int set_socket_reuse(int *socket_fd);
int bind_socket(int *socket_fd, struct sockaddr_in *addr);
int start_listening(int *socket_fd, int backlog);

#endif
