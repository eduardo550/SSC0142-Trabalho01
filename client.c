#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4206

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("Usage: %s ServerIP\n", argv[0]);
        return 0;
    }

    // Criar socket
    int sock;
    struct sockaddr_in server_address;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Error creating socket. Aborting\n");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    //argv[1] contem o endereco IP do servidor, fornecido na linha de comando
    if(inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        printf("Not a valid IP address. Aborting.\n");
        return 1;
    }

    // Fazendo a conexao
    if(connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("Connection failed. Aborting.\n");
        return 1;
    }
    printf("Connected.\n");
    // Criar funcao para troca de msgs

    close(sock);
    return 0;
}