#include "client/client_session.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>

int client_session(int sock) {
    char buf[1024];
    ssize_t n;
    fd_set lobbyfds;

    printf("You are now in the lobby. Type 'start' when ready.\n");

    while (1) {
        FD_ZERO(&lobbyfds);
        FD_SET(sock, &lobbyfds);
        FD_SET(STDIN_FILENO, &lobbyfds);

        int maxfd = sock;

        if (select(maxfd + 1, &lobbyfds, NULL, NULL, NULL) < 0) {
            perror("select");
            return -1;
        }

        // Server message
        if (FD_ISSET(sock, &lobbyfds)) {
            n = recv(sock, buf, sizeof(buf)-1, 0);
            if (n <= 0) {
                printf("Server disconnected.\n");
                return -1;
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
    
    return 0;
}
