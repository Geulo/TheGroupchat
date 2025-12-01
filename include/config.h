#ifndef CONFIG_H
#define CONFIG_H

#include <netinet/in.h>
#include <stddef.h>

#define MAX_CLIENTS 10
#define REQUIRED_CLIENTS 2
#define NICKNAME_SIZE 32
#define BUFFER_SIZE 1024

// Configuration structs
typedef struct {
    int server_fd;
    int *clients;
    char (*nicknames)[NICKNAME_SIZE];
    int required_clients;
    int max_clients;
    char *buf;
    size_t buf_size;
} LobbyConfig;

typedef struct {
    int *clients;
    char (*nicknames)[NICKNAME_SIZE];
    int max_clients;
    char *buf;
    size_t buf_size;
} ChatConfig;

typedef struct {
    int *clients;
    int max_clients;
    int server_fd;
} ServerCleanupConfig;

// ADD THIS LINE:
int configure_address(struct sockaddr_in *addr, int port);

#endif
