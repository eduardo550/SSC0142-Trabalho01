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
#define FLAGS 0
//troca de msgs

void *handle_connection(void *args) {
    pthread_detach(pthread_self());
    int sockfd = *((int *) args);
    ssize_t len;
    char msg[10];

    len = recv(sockfd, msg, 10, FLAGS);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    // Criar socket
    int serv_sock = -1, incoming_sock, incoming_len;
    int opt = 1;
    struct sockaddr_in server_address, client_address;
    pthread_t connection_thread;

    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket. Aborting.\n");
        return 1;
    }
    
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        printf("%s\n", strerror(errno));
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    // Bind
    if(bind(serv_sock, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        printf("Error binding socket. %s\n", strerror(errno));
        return 1;
    }

    // Listen
    if(listen(serv_sock, 10) == -1) {  //Backlog de 10 conexoes
        printf("Failed to listen to connections. %s\n", strerror(errno));
        return 1;
    }
    printf("Server online and awaiting connections.\n");

    // Accept
    while(1) {
        incoming_sock = accept(serv_sock, (struct sockaddr *) &client_address, &incoming_len);
        if(incoming_sock == -1) {
            printf("Connection failed to accept. %s\n", strerror(errno));
        } else {
        // fazer a troca de msgs em uma thread
            pthread_create(&connection_thread, NULL, handle_connection, (void *) &incoming_sock);
        }
    }

    close(serv_sock);
    return 0;
}