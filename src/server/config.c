#include "config.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

int configure_address(struct sockaddr_in *addr, int port) {
    if (addr == NULL) {
        perror("ERROR CODE: addr = NULL function configure_address");
        return -1;
    }
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;

    return 0;
}