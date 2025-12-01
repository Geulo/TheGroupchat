#include "client/client_messaging.h"
#include "client/client_ui.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <ctype.h>
#include <termios.h>

void handle_server_message(int sock, char *input_buf, int input_len, int *running) {
    char buf[1024];
    ssize_t n;
    
    n = recv(sock, buf, sizeof(buf)-1, 0);
    if (n <= 0) {
        printf("\nDisconnected from server.\n");
        *running = 0;
        return;
    }
    
    buf[n] = '\0';

    printf("\r\033[K");

    // Print incoming message
    printf("%s", buf);
    if (buf[n-1] != '\n') {
        printf("\n");
    }

    // Redraw prompt + user input
    printf("> %.*s", input_len, input_buf);
    fflush(stdout);
}

void handle_user_input(int sock, char *input_buf, int *input_len, int *running) {
    unsigned char c;
    
    if (read(STDIN_FILENO, &c, 1) <= 0) {
        *running = 0;
        return;
    }

    if (c == '\r' || c == '\n') {
        // Enter: send the current input buffer as a message
        printf("\r\033[K");
        printf("[You]: %.*s\n", *input_len, input_buf);

        if (*input_len > 0) {
            char send_buf[1024];
            int len = (*input_len < (int)sizeof(send_buf)-2) ? *input_len : (int)sizeof(send_buf)-2;
            memcpy(send_buf, input_buf, len);
            send_buf[len] = '\n';
            send_buf[len+1] = '\0';
            send(sock, send_buf, len+1, 0);
        }

        // Reset input
        *input_len = 0;
        input_buf[0] = '\0';

        // New prompt
        printf("> ");
        fflush(stdout);
    }
    else if (c == 0x7f || c == 0x08) {
        // Backspace
        if (*input_len > 0) {
            (*input_len)--;
            input_buf[*input_len] = '\0';

            // Redraw line
            printf("\r\033[K> %.*s", *input_len, input_buf);
            fflush(stdout);
        }
    }
    else if (isprint(c) && *input_len < 1023) {
        // Printable character
        input_buf[(*input_len)++] = c;
        input_buf[*input_len] = '\0';

        // Echo it
        printf("%c", c);
        fflush(stdout);
    }
}

void handle_chat(int sock) {
    printf("You can now type messages.\n");

    struct termios old_tio;
    set_raw_mode(&old_tio);

    fd_set readfds;
    char input_buf[1024];
    int input_len = 0;

    printf("> ");
    fflush(stdout);

    int running = 1;

    while (running) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Server sent a message
        if (FD_ISSET(sock, &readfds)) {
            handle_server_message(sock, input_buf, input_len, &running);
        }

        // User typed something
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            handle_user_input(sock, input_buf, &input_len, &running);
        }
    }

    // Restore terminal
    restore_mode(&old_tio);
}
