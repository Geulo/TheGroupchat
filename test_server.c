# include <arpa/inet.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <time.h>
# include <unistd.h>

# define ISVALIDSOCKET(s) ((s) >= 0)
# define SOCKET int

//--definitions--
typedef struct{

    char PORT_NUMBER[6];

} configuration_server;

struct addrinfo *get_bind_address(const char *port) //converting the port string into vliud
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints. = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE; // Letting OS choose IP: For wildcard IP address
    struct addrinfo *bind_address;
    getaddrinfo(0, port, &hints, &bind_address);
    return bind_address;
}

//--declerations-- 
SOCKET create_listening_socket(const char *port);

SOCKET accept_client(SOCKET socket_listen);

SOCKET handle_clinet(SOCKET socket_client);

SOCKET create_socket(struct ADDRESS_INFO *ADDRESS_INFO);

//--main--
int main()
{
    configuration_server DEFAULT = {
        .PORT_NUMBER = "8080"
    };

    SOCKET socket_listen = create_listening_socket(&DEFAULT);
    SOCKET socket_client = accept_client(socket_listen);

    if (!ISVALIDSOCKET(socket_client)){return 1;}

    handle_clinet(socket_client);
    print("closing connection");
    close(socket_client);
    close(socket_listen);

    return 0;
}


//--function definitions--
SOCKET create_listening_socket(const char *port)
{
    struct addressinfo *bind_address = get_bind_address(port); //receiving and putting the port number into the addrinfo

    SOCKET socket_listen = create_socket(bind_address); // creating the soket at bind_address

    bind_socket(socket_listen, bind_address); //binding the socket to the listen 

    freeaddrinfo(bind_address);

    printf("Listenin$g...\n");

    if (listen(socket_listen, 10) < 0) {
    fprintf(stderr, "listen() failed. (%s)\n", strerror(errno));
    exit(1);

    }
}

SOCKET create_socket(struct addrinfo *address_info) {
    printf("Creating socket...\n");
    SOCKET sock;    
    sock = socket(address_info->ai_family, address_info->ai_socktype,
    address_info->ai_protocol);
    if (!ISVALIDSOCKET(sock)) {
        fprintf(stderr, "socket() failed. (%s)\n", strerror(errno));
        exit(1);
    }
    return sock;
}

