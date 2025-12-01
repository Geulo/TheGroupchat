#ifndef CLIENT_MESSAGING_H
#define CLIENT_MESSAGING_H

void handle_server_message(int sock, char *input_buf, int input_len, int *running);
void handle_user_input(int sock, char *input_buf, int *input_len, int *running);
void handle_chat(int sock);

#endif
