#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define PORT 4206
#define FLAGS 0
//troca de msgs

int create_account(char *username) {
    char path[20] = "db/";
    struct stat s;
    strcat(path, username);

    if (stat(path, &s) != -1) {
        printf("Account already exists.\n");
        return -1;
    } else {
        mkdir(path, 0777);
        return strlen(username);
    }
}

void handle_login(int sockfd) {
    ssize_t user_len;
    char username[17];
    int username_option;
    int errorcode = -1;
    int retval;

    //Recebendo o modo, criar conta ou login
    recv(sockfd, &username_option, sizeof(int), FLAGS);
    switch(username_option) {
        case 0:
            user_len = recv(sockfd, username, 17, FLAGS);
            if(user_len == -1) {
                send(sockfd, &errorcode, sizeof(int), FLAGS);
            } else {
                retval = create_account(username);
                send(sockfd, &retval, sizeof(int), FLAGS);
            }
            break;
        case 1:
            user_len = recv(sockfd, username, 17, FLAGS);
            break;
        default:
            send(sockfd, &errorcode, sizeof(int), FLAGS);
            break;
    }

}

void *handle_connection(void *args) {
    //Deixa a thread independente, para que quando a conexao terminar, ela libera seus recursos automaticamente
    pthread_detach(pthread_self());
    int sockfd = *((int *) args);

    handle_login(sockfd);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    // Criar socket
    int serv_sock = -1, incoming_sock;
    socklen_t incoming_len;
    int opt = 1;
    struct sockaddr_in server_address, client_address;
    struct stat db;
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

    //Criando a pasta que guarda os arquivos
    if(stat("db", &db) == -1) {
        if (mkdir("db", 0777) == -1) {
            printf("Error creating database\n");
            return 1;
        } else {
            printf("Database initialized.\n");
        }
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