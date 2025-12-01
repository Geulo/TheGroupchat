#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <termios.h>
#include <ctype.h>
#include "client/client_ui.h"
#include "client/client_messaging.h"
#include "client/client_session.h"
#include "client/client_ui.h"
#include "client/client.h"
#include "client/client_network.h"

int chat_client_run(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];

    // Create and connect socket
    int sock = create_and_connect_socket(server_ip);
    if (sock < 0) {
        return 1;
    }

    // Get and send nickname
    if (get_and_send_nickname(sock) < 0) {
        close(sock);
        return 1;
    }

    // Handle lobby mode
    if (client_session(sock) < 0) {
        close(sock);
        return 1;
    }

    // Handle chat mode
    handle_chat(sock);

    close(sock);
    return 0;

}
