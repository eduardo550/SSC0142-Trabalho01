#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define PORT 4206
//troca de msgs

int main(int argc, char *argv[]) {

    // Criar socket
    int serv_sock = -1, incoming_sock, incoming_len;
    struct sockaddr_in server_address, client_address;

    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket. Aborting.\n");
        return 1;
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    // Bind
    if(bind(serv_sock, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        printf("Error binding socket.\n");
        return 1;
    }

    // Listen
    if(listen(serv_sock, 10) != 0) {  //Backlog de 10 conexoes
        printf("Failed to listen to connections.\n");
        return 1;
    }
    printf("Server online and awaiting connections.\n");

    // Accept
    while(1) {
        incoming_sock = accept(serv_sock, (struct sockaddr *) &client_address, &incoming_len);
        if(incoming_sock < 0) {
            printf("Connection failed to accept.\n");
        } else {
        // fazer a troca de msgs em uma thread
            printf("Connected to %s.\n", inet_ntoa(client_address.sin_addr));
        }
    }

    close(serv_sock);
    return 0;
}