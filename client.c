#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define PORT 4206
#define FLAGS 0

int handle_connection(int sockfd) {
    //Exemplo comunicaçao
    // ssize_t len;
    // char *msg = "123";
    // int flags = 0;
    // len = send(sockfd, msg, 4, FLAGS);

    char username[17]; //16 + '\0'
    /*Interface:
        Inicialemente pergunta se quer criar uma conta ou logar
        Conta só tem username, de no max 16 chars
        Mandar o username, servidor retorna 0 se sucesso, -1 se a conta existe no caso de criar, ou se n existe no caso de logar
        Depois, um prompt com opçoes
            1. Listar os arquivos no server
            2. Mandar um arquivo para o sr
            3. Copiar um arquivo do server
            4. Remover um arquivo do server
            5. Remover todos os arquivos do server
            6. Remover a conta
            7. Sair
        Faz um send com o n da opcao, e outro com o arg, se necessario
        Args:
            1. Nenhum
            2. O arquivo
            3. O nome do arquivo
            4. O nome do arquivo
            5. Nenhum
            6. Nenhum
            7. Nenhum, termina a conexao e programa acaba
        Servidor retorna:
            1. 0 (sucesso) ou -1 (fail)
            2. n_bytes (sucesso) ou -1 (fail)
            3. o arquivo (sucesso) ou -1 (fail)
            4. 0 (sucesso) ou -1 (fail)
            5. 0 (sucesso) ou -1 (fail)
            6. 0 (sucesso) ou -1 (fail)
            7. Nada, programa acaba
    */

    //Case 7
    //Um print aqui tbm
    close(sockfd);
    return 0;
}

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
        printf("Connection failed. Aborting. %s\n", strerror(errno));
        return 1;
    }
    printf("Connected.\n");
    // Criar funcao para troca de msgs
    handle_connection(sock);


    return 0;
}