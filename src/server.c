#include "server.h"
#include "network.h"
#include "config.h"
#include "client_manager.h"
#include <stdio.h>
#include <unistd.h>

int chat_server_run(void) {
    
    //creating socket
    int server_fd;
    if (create_socket(&server_fd) < 0) {return 1;} // 1 = socket creation failed

    // Allow port reuse
    if (set_socket_reuse(&server_fd) < 0) {close(server_fd); perror("ERROR CODE: reuse setting failed, function set_socket_reuse"); return 1;} // 1 = reuse setting failed

    
    // turning server configuration into compiler readable code
    struct sockaddr_in addr;    //initializing struct to hold server addr config
    if (configure_address(&addr, 8080) < 0 ){perror("ERROR CODE: function configure_address"); close(server_fd); return 1;}
                            //make 8080 editable outside this
        
    // binding the socket to the address
    if (bind_socket(&server_fd, &addr) < 0) {close(server_fd); return 1;}

    // start to listen to connections on specificied socket
    if (start_listening(&server_fd, 5) < 0) {close(server_fd); return 1;}
    

    printf("Server listening on port 8080...\n");

    int clients[MAX_CLIENTS];
    char nicknames[MAX_CLIENTS][32];

    initialize_clients(clients, nicknames, MAX_CLIENTS);

    char buf[1024];


    // ______LOBBY START____ //

    // Setup lobby configuration
    LobbyConfig lobby_config = {
        .server_fd = server_fd,
        .clients = clients,
        .nicknames = nicknames,
        .required_clients = REQUIRED_CLIENTS,
        .max_clients = MAX_CLIENTS,
        .buf = buf,
        .buf_size = sizeof(buf)
    };

    // Run lobby
    int lobby_count = accept_clients(&lobby_config);

    if (lobby_count < 0) {
    perror("ERROR CODE: function accept_clients");
    close(server_fd);
    return 1;
    }
 
    // ______ LOBBY END ____ //

    // Checking and handling client messaging

    ChatConfig chat_config = {
    .clients = clients,
    .nicknames = nicknames,
    .max_clients = MAX_CLIENTS,
    .buf = buf,
    .buf_size = sizeof(buf)
    };

    while (1) {handle_client_messages(&chat_config);}

    // Cleaning up the server
    ServerCleanupConfig cleanup_config = {
    .clients = clients,
    .max_clients = MAX_CLIENTS,
    .server_fd = server_fd
    };

    cleanup_server(&cleanup_config);
    close(server_fd);

    return 0;
}