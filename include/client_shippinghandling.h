#ifndef CLIENT_LOBBY_H
#define CLIENT_LOBBY_H

#include <stddef.h>

// Lobby configuration
typedef struct {
    int socket_fd;
    char *buffer;
    size_t buffer_size;
} LobbyConfig;

// Get nickname from user
int get_nickname(char *nickname, size_t size);

// Send nickname to server
int send_nickname(int socket_fd, const char *nickname);

// Run lobby phase (wait for START)
int run_lobby(LobbyConfig *config);

#endif