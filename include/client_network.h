#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

int connect_to_server(const char *server_ip, int port);
int send_nickname(int sock, const char *nickname);

#endif