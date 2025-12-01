#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

int create_and_connect_socket(const char *server_ip);
int get_and_send_nickname(int sock);

#endif 
