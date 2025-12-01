#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include "config.h"

void initialize_clients(int *clients, char nicknames[][NICKNAME_SIZE], int max_clients);
int accept_clients(LobbyConfig *config);
void handle_client_messages(ChatConfig *config);
void cleanup_server(ServerCleanupConfig *config);

#endif
