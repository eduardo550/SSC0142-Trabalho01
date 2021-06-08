/* Trabalho 01 - Servidor FTP Simples
	Eduardo de Sousa Siqueira		9278299
	Igor Barbosa Grécia Lúcio		9778821
*/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

//Maximo de bytes que sao enviados de uma vez
#define MAX 80
//Flags para funcoes send() e recv()
#define FLAGS 0
#define SA struct sockaddr

//Funcao que cria o diretorio de um novo usuario
int create_account(char *username) {
    char path[20] = "db/";
    struct stat s;
    strcat(path, username);

    if (stat(path, &s) != -1) {
        return -1;
    } else {
        mkdir(path, 0777);
        return strlen(username);
    }
}

//Funcao para envio de arquivos para o cliente
void send_file(FILE *fp, int sockfd){
	char data[MAX] = {0};
	while(fgets(data, MAX, fp) != NULL) {
		if (send(sockfd, data, sizeof(data), 0) == -1) {
			exit(1);
		}
		bzero(data, MAX);
	}
}

//Funcao que deleta todos os arquivos pertencentes a um usuario
void delete_files_from_user(char *path) {
	DIR *d = opendir(path);
	char file_path[40];
	struct dirent *dir;

	//Loop pegando todos os nomes de arquivos no diretorio e os removendo
	while ((dir = readdir(d)) != NULL){
		bzero(file_path, 40);
		strcat(file_path, path);
		strcat(file_path, "/");
		strcat(file_path, dir->d_name);
		remove(file_path);
	}

	closedir(d);
}

//Funcao que lida com a comunicacao com o programa cliente
void *handle_connection(void *args) {
	//Tornando a thread independente do programa principal. Ao final da conexao, ela libera seus recursos automaticamente
	pthread_detach(pthread_self());

	char buff[MAX];
	char path[20];
	char file_path[40];
	FILE *fp;
	DIR *d, *p;
	struct dirent *dir;
	int retval;
	int sockfd = *((int *) args);

	//Loop infinito para receber as opcoes do cliente
	for (;;) {
		bzero(buff, MAX);
		//Lendo a opcao do cliente - login, criar conta ou sair
		read(sockfd, buff, sizeof(buff));

		// 1 - Fazer Login
		if (strncmp("1\n", buff, 2) == 0) {
			bzero(buff, MAX);
			bzero(path, 20);
			bzero(file_path, 40);
			strcat(path,"db/");
			read(sockfd, buff, sizeof(buff));
			strcat(path, buff);
			p = opendir(path);
			if (p) {
				write(sockfd, buff, sizeof(buff)); 
				retval=strlen(buff);
				send(sockfd, &retval, sizeof(int), FLAGS);
				for(;;){
					//Option Selector
					read(sockfd, buff, sizeof(buff));
					//Option Selector 1 - List Files
					if (strncmp("1\n", buff, 2) == 0) {
						d = opendir(path);
						//Le cada nome de arquivo no diretorio que corresponde ao username do cliente e envia para impressao no cliente
					    while ((dir = readdir(d)) != NULL){
					    	strcpy(buff,dir->d_name);
				        	write(sockfd, buff, sizeof(buff));
				        	bzero(buff, MAX);
				    	}
				    	closedir(d);
						//Envia a mensagem dizendo que terminou a listagem
						write(sockfd, "-1", 2);
					}
					//Option Selector 2 - Receive File from Client
					else if (strncmp("2\n", buff, 2) == 0) {
						read(sockfd, buff, sizeof(buff));
						//Recebido um nome valido, procede a receber os dados
						//A string \\/ eh uma mensagem de erro que o cliente envia quando ha erro de leitura no seu lado
						if(strcmp("\\\\//", buff)) {
							printf("Receiving\n");
							bzero(file_path, 40);
							strcat(file_path, path);
							strcat(file_path, "/");
							strcat(file_path, buff);
							fp = fopen(file_path, "w");
							bzero(buff, MAX);
							read(sockfd, buff, sizeof(buff));
							fprintf(fp, "%s", buff);
							bzero(buff, MAX);
							fclose(fp);
						}
					}
					//Option Selector 3 - Send File to Client
					else if (strncmp("3\n", buff, 2) == 0) {
						bzero(buff, sizeof(buff));
						read(sockfd, buff, sizeof(buff));
						bzero(file_path, 40);
						strcat(file_path, path);
						strcat(file_path, "/");
						strcat(file_path, buff);
						fp = fopen(file_path, "r");
						//Envia apenas se o arquivo existe, senao envia uma mensagem de erro
						if (fp == NULL) {
							char *errval = "Not found";
							send(sockfd, errval, strlen(errval)+1, FLAGS);
						} else {
							send_file(fp, sockfd);
							fclose(fp);
						}
					}
					//Option Selector 4 - Remove Single File
					else if (strncmp("4\n", buff, 2) == 0) {
						//Recebe o nome do arquivo e o deleta
						read(sockfd, buff, sizeof(buff));
						bzero(file_path, 40);
						strcat(file_path, path);
						strcat(file_path, "/");
						strcat(file_path, buff);
						remove(file_path);
					}
					//Option Selector 5 - Remove All Files
					else if (strncmp("5\n", buff, 2) == 0) {
						delete_files_from_user(path);
					}
					//Option Selector 6 - Remove Account
					else if (strncmp("6\n", buff, 2) == 0) {
						delete_files_from_user(path);
						strcat(file_path, path);
						strcat(file_path, "/");
						rmdir(file_path);
						closedir(p);
						break;
					}
					//Option Selector 7 - Logout
					else if (strncmp("7\n", buff, 2) == 0) {
						closedir(p);
						break;
					}
				}
			} else {
				retval = -1;
				send(sockfd, &retval, sizeof(int), FLAGS);
			}
		}
		// 0 - Criar Conta
		else if (strncmp("0\n", buff, 2) == 0) {
			//Lendo o novo usuario e criando a conta
			read(sockfd, buff, sizeof(buff));
			retval = create_account(buff);
			//Retorna um codigo de sucesso ou erro para o cliente, definido na funcao create account
            send(sockfd, &retval, sizeof(int), FLAGS);
		}
		// 7 - Fechar Conexao
		else if (strncmp("7\n", buff, 2) == 0) {
			break;
		}
	}

	pthread_exit(NULL);
}

// Driver function
int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage:\n\t%s [Server Port]\n", argv[0]);
		exit(1);
	}

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	struct stat db;
	pthread_t connection;

	//Criando socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(1);
	}
	printf("Socket successfully created..\n");
	//Zerando enderecos, para garantir preenchimento correto
	bzero(&servaddr, sizeof(servaddr));

	//Servidor sera ligado a todas as interfaces de rede do sistema
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//Port do servidor, recebido da linha de comando
	servaddr.sin_port = htons(atoi(argv[1]));

	//Ligando server ao local definido
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(1);
	}
	printf("Socket successfully bound to port %s..\n", argv[1]);


	//Colocando o servidor em modo passivo, para receber conexoes 
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(1);
	}
	printf("Server listening..\n");

    //Criando o diretorio de armazenamento
    if(stat("db", &db) == -1) {
        if (mkdir("db", 0777) == -1) {
            printf("Error creating database\n");
            return 1;
        } else {
            printf("Database initialized.\n");
        }
    }

	//Loop de funcionamento. Recebe conexoes continuamente, criando threads para sua execucao
	while(1) {
		connfd = accept(sockfd, (SA*)&cli, (socklen_t *) &len);
		if (connfd < 0) {
			printf("server acccept failed...\n");
		}
		else {
			printf("server acccept the client...\n");
			//Cada conexao é tratada em uma thread, que libera seus recursos no fim da interacao com o cliente
			pthread_create(&connection, NULL, handle_connection, (void *) &connfd);
		}
	}

	return 0;
}