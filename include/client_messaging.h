#ifndef CLIENT_CHAT_H
#define CLIENT_CHAT_H

#include <stddef.h>
#include <termios.h>

// Chat configuration
typedef struct {
    int socket_fd;
    char *buffer;
    size_t buffer_size;
    struct termios *old_terminal;
} ChatConfig;

// Run main chat loop
void run_chat(ChatConfig *config);

// Handle incoming server messages
void handle_server_message(int socket_fd, char *buf, size_t buf_size, 
                          char *input_buf, int input_len);

// Handle user input (character-by-character)
int handle_user_input(int socket_fd, char *input_buf, int *input_len, 
                      size_t max_len);

#endif