#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <termios.h>
#include <ctype.h>

static void set_raw_mode(struct termios *old_tio) {
    struct termios new_tio;

    if (tcgetattr(STDIN_FILENO, old_tio) < 0) {
        perror("tcgetattr");
        exit(1);
    }

    new_tio = *old_tio;
    // Turn off canonical mode and echo
    new_tio.c_lflag &= ~(ICANON | ECHO);
    // Optional: keep signals (Ctrl-C) working
    // new_tio.c_lflag &= ~ISIG; // uncomment if you *don't* want signals

    new_tio.c_cc[VMIN]  = 1;
    new_tio.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) < 0) {
        perror("tcsetattr");
        exit(1);
    }
}

static void restore_mode(const struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);

    printf("Connecting to %s:8080...\n", server_ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected!\n");

    // ASK FOR NICKNAME
    char nickname[32];
    printf("Enter your nickname: ");
    fflush(stdout);
    if (!fgets(nickname, sizeof(nickname), stdin)) {
        close(sock);
        return 1;
    }

    // remove newline
    nickname[strcspn(nickname, "\n")] = 0;

    // send nickname to server
    send(sock, nickname, strlen(nickname), 0);
    send(sock, "\n", 1, 0); // end with newline

    // -------- LOBBY MODE (line-based, fgets, normal terminal) --------
    char buf[1024];
    ssize_t n;

    fd_set lobbyfds;

    printf("You are now in the lobby. Type 'start' when ready.\n");

    while (1) {
        FD_ZERO(&lobbyfds);
        FD_SET(sock, &lobbyfds);          // server messages
        FD_SET(STDIN_FILENO, &lobbyfds);  // user input

        int maxfd = sock;

        if (select(maxfd + 1, &lobbyfds, NULL, NULL, NULL) < 0) {
            perror("select");
            close(sock);
            return 1;
        }

        // Server message
        if (FD_ISSET(sock, &lobbyfds)) {
            n = recv(sock, buf, sizeof(buf)-1, 0);
            if (n <= 0) {
                printf("Server disconnected.\n");
                close(sock);
                return 1;
            }

            buf[n] = '\0';
            printf("Server: %s", buf);

            if (strcmp(buf, "START\n") == 0) {
                printf("Lobby finished. Entering chat...\n");
                break;
            }
        }

        // User typed something (like "start")
        if (FD_ISSET(STDIN_FILENO, &lobbyfds)) {
            if (!fgets(buf, sizeof(buf), stdin))
                continue;
            send(sock, buf, strlen(buf), 0);
        }
    }

    // -------- CHAT MODE (raw input, no losing what you type) --------
    printf("You can now type messages.\n");

    struct termios old_tio;
    set_raw_mode(&old_tio);   // switch to raw-ish mode

    fd_set readfds;
    char input_buf[1024];
    int  input_len = 0;

    // show initial prompt
    printf("> ");
    fflush(stdout);

    int running = 1;

    while (running) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);          // server messages
        FD_SET(STDIN_FILENO, &readfds);  // user input

        int maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // ---- Server sent a message ----
        if (FD_ISSET(sock, &readfds)) {
            n = recv(sock, buf, sizeof(buf)-1, 0);
            if (n <= 0) {
                // Server closed or error
                printf("\nDisconnected from server.\n");
                break;
            }
            buf[n] = '\0';

            // Clear current input line
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

        // ---- User typed something (raw mode, char-by-char) ----
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            unsigned char c;
            if (read(STDIN_FILENO, &c, 1) <= 0) {
                running = 0;
                break;
            }

            if (c == '\r' || c == '\n') {
                // Enter: send the current input buffer as a message
                printf("\r\033[K"); // clear line
                printf("[You]: %.*s\n", input_len, input_buf);

                if (input_len > 0) {
                    char send_buf[1024];
                    int len = (input_len < (int)sizeof(send_buf)-2) ? input_len : (int)sizeof(send_buf)-2;
                    memcpy(send_buf, input_buf, len);
                    send_buf[len] = '\n';
                    send_buf[len+1] = '\0';
                    send(sock, send_buf, len+1, 0);
                }

                // Reset input
                input_len = 0;
                input_buf[0] = '\0';

                // New prompt
                printf("> ");
                fflush(stdout);
            }
            else if (c == 0x7f || c == 0x08) {
                // Backspace
                if (input_len > 0) {
                    input_len--;
                    input_buf[input_len] = '\0';

                    // Redraw line
                    printf("\r\033[K> %.*s", input_len, input_buf);
                    fflush(stdout);
                }
            }
            else if (isprint(c) && input_len < (int)sizeof(input_buf)-1) {
                // Printable character
                input_buf[input_len++] = c;
                input_buf[input_len] = '\0';

                // Echo it
                printf("%c", c);
                fflush(stdout);
            }
            // Ignore other control characters
        }
    }

    // restore terminal
    restore_mode(&old_tio);

    close(sock);
    return 0;
}
