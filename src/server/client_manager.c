#include "server/client_manager.h"
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

void initialize_clients(int *clients, char nicknames[][NICKNAME_SIZE], int max_clients) {
    for (int i = 0; i < max_clients; i++) {
        clients[i] = -1;
        nicknames[i][0] = '\0';
    }
}

int accept_clients(LobbyConfig *config) {
    if (config == NULL || config->clients == NULL || config->buf == NULL) {
        perror("ERROR CODE: Invalid lobby configuration");
        return -1;
    }

    int lobby_count = 0;

    printf("Lobby: waiting for %d clients...\n", config->required_clients);

    while (lobby_count < config->required_clients) {
        int fd = accept(config->server_fd, NULL, NULL);
        if (fd < 0) continue;

        config->clients[lobby_count] = fd;

        printf("Lobby: client fd=%d joined (%d/%d)\n",
               fd, lobby_count+1, config->required_clients);

        ssize_t nn = recv(fd, config->nicknames[lobby_count],
                          sizeof(config->nicknames[lobby_count]) - 1, 0);

        if (nn > 0) {
            if (config->nicknames[lobby_count][nn - 1] == '\n')
                config->nicknames[lobby_count][nn - 1] = '\0';
            else
                config->nicknames[lobby_count][nn] = '\0';
        } else {
            strcpy(config->nicknames[lobby_count], "Unknown");
        }

        printf("Lobby nickname for slot %d: %s\n",
               lobby_count, config->nicknames[lobby_count]);

        send(fd, "WAITING\n", 8, 0);
        lobby_count++;
    }

    printf("Lobby full! Waiting for someone to type 'start'...\n");

    int start_received = 0;

    while (!start_received) {
        for (int i = 0; i < lobby_count; i++) {
            ssize_t n = recv(config->clients[i], config->buf, config->buf_size - 1, MSG_DONTWAIT);
            if (n > 0) {
                config->buf[n] = '\0';
                printf("Lobby message from fd=%d: %s", config->clients[i], config->buf);

                if (strncmp(config->buf, "start", 5) == 0) {
                    start_received = 1;
                    break;
                }
            }
        }
        usleep(20000);
    }

    printf("START received! Beginning chat.\n");

    for (int i = 0; i < lobby_count; i++) {
        send(config->clients[i], "START\n", 6, 0);
    }

    return lobby_count;
}

void handle_client_messages(ChatConfig *config) {
    fd_set read_fds;
    FD_ZERO(&read_fds);

    int max_fd = 0;

    for (int i = 0; i < config->max_clients; i++) {
        if (config->clients[i] != -1) {
            FD_SET(config->clients[i], &read_fds);
            if (config->clients[i] > max_fd) {
                max_fd = config->clients[i];
            }
        }
    }

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
        perror("select");
        return;
    }

    for (int i = 0; i < config->max_clients; i++) {
        if (config->clients[i] != -1 && FD_ISSET(config->clients[i], &read_fds)) {
            ssize_t n = recv(config->clients[i], config->buf, config->buf_size - 1, 0);
            
            if (n <= 0) {
                printf("Client disconnected: fd=%d (slot %d) nickname=%s\n", 
                       config->clients[i], i, config->nicknames[i]);
                close(config->clients[i]);
                config->clients[i] = -1;
                config->nicknames[i][0] = '\0';
            } else {
                config->buf[n] = '\0';
                printf("[%s] says: %s", config->nicknames[i], config->buf);
                
                char msg[1100];
                snprintf(msg, sizeof(msg), "[%s]: %s", config->nicknames[i], config->buf);
                
                for (int j = 0; j < config->max_clients; j++) {
                    if (config->clients[j] != -1 && j != i) {
                        send(config->clients[j], msg, strlen(msg), 0);
                    }
                }
            }
        }
    }
}

void cleanup_server(ServerCleanupConfig *config) {
    if (config == NULL) {
        return;
    }
    
    for (int i = 0; i < config->max_clients; i++) {
        if (config->clients[i] != -1) {
            printf("Closing client fd=%d (slot %d)\n", config->clients[i], i);
            close(config->clients[i]);
            config->clients[i] = -1;
        }
    }
    
    printf("Closing server socket fd=%d\n", config->server_fd);
    close(config->server_fd);
    
    printf("Server shutdown complete.\n");
}
