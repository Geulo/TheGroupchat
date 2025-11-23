#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct configuration
{
    char IP_TYPE; 
    char SOCKET_TYPE;

} configuration;

int opening_socket(struct configuration *config);
int closing_socket(int *sock);

int main(void)
{
    configuration DEFAULT = {
        .IP_TYPE = AF_INET, 
        .SOCKET_TYPE = SOCK_STREAM
    };

    int SOCKET_OPEN = opening_socket(&DEFAULT);
    int *SOCKET_ptr = &SOCKET_OPEN;
    int SOCKET_CLOSE = closing_socket(SOCKET_ptr);

    return 0;

}




int opening_socket(struct configuration *config)
{
    int socketfd = socket(config->IP_TYPE, config->SOCKET_TYPE, 0);

    if (socketfd == -1){perror("socketFailure"); exit(EXIT_FAILURE);}

    printf("\nSOCKET OPEN");
    return socketfd;
}

int closing_socket(int *sock)
{
    if (close(*sock) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
           
        }
    printf("\nSOCKET CLOSED");
    return 0;
}                   
