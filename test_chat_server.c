#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10

int create_socket(int *server_fd)
{
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (*server_fd < 0) {
        perror("ERROR CODE: function create_socket");
        return -1;
    }

    return 0;
}

int set_socket_reuse(int *socket_fd)
{
    int opt = 1;
    if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("ERROR CODE: function set_socket_reuse");
        return -1; // FAIL
    }

    return 0; // SUCCESS
}

int chat_server_run(void) {
    
    //creating socket
    int server_fd;
        if (create_socket(&server_fd) < 0) {return 1;} // 1 = socket creation failed

    // Allow port reuse
    if (set_socket_reuse(&server_fd) < 0) {close(server_fd); perror("ERROR CODE: reuse setting failed, function set_socket_reuse"); return 1;} // 1 = reuse setting failed

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port 8080...\n");

    int clients[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;
    }

    char buf[1024];

    while (1) {

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);

        int max_fd = server_fd;

        // Add all active clients to the set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != -1) {
                FD_SET(clients[i], &read_fds);
                if (clients[i] > max_fd) {
                    max_fd = clients[i];
                }
            }
        }
        
        

        // Wait for activity on any socket
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Check if there's a new connection
        if (FD_ISSET(server_fd, &read_fds)) {
            int new_client = accept(server_fd, NULL, NULL);
            if (new_client < 0) {
                perror("accept");
            } else {
                // Find a slot for the new client
                int added = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] == -1) {
                        clients[i] = new_client;
                        printf("New client connected: fd=%d (slot %d)\n", new_client, i);
                        
                        const char *welcome = "Welcome to the chat server!\n";
                        send(new_client, welcome, strlen(welcome), 0);
                        added = 1;
                        break;
                    }
                }
                if (!added) {
                    printf("Max clients reached, rejecting connection\n");
                    const char *msg = "Server full. Try again later.\n";
                    send(new_client, msg, strlen(msg), 0);
                    close(new_client);
                }
            }
        }

        // Check all clients for incoming messages
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != -1 && FD_ISSET(clients[i], &read_fds)) {
                ssize_t n = recv(clients[i], buf, sizeof(buf) - 1, 0);
                
                if (n <= 0) {
                    // Client disconnected
                    printf("Client disconnected: fd=%d (slot %d)\n", clients[i], i);
                    close(clients[i]);
                    clients[i] = -1;
                } else {
                    buf[n] = '\0';
                    printf("Client %d says: %s", clients[i], buf);
                    
                    // Broadcast message to all other clients
                    char msg[1100];
                    snprintf(msg, sizeof(msg), "Client %d: %s", clients[i], buf);
                    
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clients[j] != -1 && j != i) {
                            send(clients[j], msg, strlen(msg), 0);
                        }
                    }
                }
            }
        }
    }

    // Cleanup
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            close(clients[i]);
        }
    }
    close(server_fd);

    return 0;
}

int main() {
    return chat_server_run();
}