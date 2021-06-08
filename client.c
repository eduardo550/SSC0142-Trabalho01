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

//Maximo de bytes que sao enviados de uma vez
#define MAX 80
//Flags para funcoes send() e recv()
#define FLAGS 0
#define SA struct sockaddr

//Funcao que trata o envio de um arquivo, em chunks de MAX == 80 bytes
void send_file(FILE *fp, int sockfd) {
	char data[MAX] = {0};

	//Lendo arquivo em segmentos de MAX, até o final
	while(fgets(data, MAX, fp) != NULL) {
		if (send(sockfd, data, sizeof(data), FLAGS) == -1) {
			perror("[-]Error in sending file.");
			exit(1);
		}
		
		bzero(data, MAX);
	}
}

void func(int sockfd) {
	char buff[MAX];
	char name[MAX];
	int n;
	int retval;
	FILE *fp;
	for(;;){
		printf("Criar Conta(0), Login(1) ou Sair do Programa(7)\n");
		bzero(buff, sizeof(buff));
		n = 0;
		//Lendo a opcao escolhida e enviando para o servidor
		while ((buff[n++] = getchar()) != '\n');
		send(sockfd, buff, sizeof(buff), FLAGS);

		//Opção 1 - Fazer Login
		if (strcmp(buff, "1\n") == 0) {
			printf("Nome de usuario: ");
			for(;;){
				//Lendo o nome de usuario
				bzero(buff, sizeof(buff));
				bzero(name, sizeof(name));
				n = 0;
				while ((buff[n++] = getchar()) != '\n');

				//Copiando para o buffer, sem a quebra de linha, e enviando para o servidor
				strncpy(name, buff, n-1);
				send(sockfd, name, sizeof(name), FLAGS);
				recv(sockfd, &retval, sizeof(int), FLAGS);

				//Recebido sucesso, entra na interface principal
				if (retval != -1) {
					printf("Logado\n\n");
					//Loop de interface principal
					for(;;) {
						printf("Escolha uma opção:\n"
							"\t1. Listar Arquivos\n"
							"\t2. Upload um Arquivo\n"
							"\t3. Download um Arquivo\n"
							"\t4. Remover um Arquivo do Servidor\n"
							"\t5. Remover Todos os seus Arquivos\n"
							"\t6. Remover sua Conta\n"
							"\t7. Logout\n"
						);
						//Lendo a opçao e enviando para o servidor
						bzero(buff, sizeof(buff));
						n = 0;
						while ((buff[n++] = getchar()) != '\n');
						send(sockfd, buff, sizeof(buff), FLAGS);

						//Option Selector 1 - List Files
						if (strncmp(buff, "1\n", 2) == 0){
							//Lendo e imprimindo a lista de arquivos contidos no server
							read(sockfd, buff, sizeof(buff));
							while(strncmp(buff, "-1", 2) != 0){
								printf("%s\n", buff);
								bzero(buff, sizeof(buff));
								read(sockfd, buff, sizeof(buff));
							}
						}
						//Option Selector 2 - Send File to Server
						else if (strncmp(buff, "2\n", 2) == 0) {
							printf("Nome do arquivo: ");
							//Lendo o nome do arquivo
							bzero(name, sizeof(name));
							bzero(buff, sizeof(buff));
							n = 0;
							while ((buff[n++] = getchar()) != '\n');
							//Copiando para o buffer sem a quebra de linha
							strncpy(name, buff, n-1);
							fp = fopen(name, "r");

							//Enviando o nome do arquivo para o servidor
							if (fp == NULL) {
								perror("[-]Error in reading file.");
								char *errval = "\\\\//";
								send(sockfd, errval, strlen(errval)+1, FLAGS);
							} else {
								send(sockfd, name, sizeof(name), 0);
								//Enviando o conteudo do arquivo
								send_file(fp, sockfd);
								fclose(fp);
								printf("%s enviado\n", name);
							}
						}
						//Option Selector 3 - Copy File from Server
						else if (strncmp(buff, "3\n", 2) == 0) {
							printf("Nome do arquivo: ");
							//Lendo o nome do arquivo
							bzero(name, sizeof(name));
							bzero(buff, sizeof(buff));
							n = 0;
							while ((buff[n++] = getchar()) != '\n');
							//Copiando para o buffer sem a quebra de linha
							strncpy(name, buff, n-1);

							send(sockfd, name, sizeof(name), FLAGS);
							recv(sockfd, buff, sizeof(buff), FLAGS);

							//Tratando a resposta do servidor
							if(!strcmp(buff, "Not found")) {
								printf("Arquivo nao foi encontrado no servidor.\n");
							} else {
								fp = fopen(name, "w");
								fprintf(fp, "%s", buff);
								fclose(fp);
								printf("Recebido\n");
							}
						}
						//Option Selector 4 - Delete Single File
						else if (strncmp(buff, "4\n", 2) == 0) {
							//Lendo o nome do arquivo a ser deletado
							printf("Nome do arquivo: ");
							bzero(name, sizeof(name));
							bzero(buff, sizeof(buff));
							n = 0;
							while ((buff[n++] = getchar()) != '\n');
							strncpy(name, buff, n-1);
							
							send(sockfd, name, sizeof(name), FLAGS);
						}
						//Option Selector 5 - Delete All Files
						else if (strncmp(buff, "5\n", 2) == 0) {
							printf("Deleting All Files\n");
						}
						//Option Selector 6 - Delete Account + Logout
						else if (strncmp(buff, "6\n", 2) == 0) {
							printf("Deleting Account\n");
							break;
						}
						//Option Selector 7 - Logout
						else if (strncmp(buff, "7\n", 2) == 0) {
							printf("Saindo...\n");
							break;
						}
						else {
							printf("Opcao nao reconhecida\n");
						}
					}
					break;
				}
				else {
					printf("Conta inexistente.\n");
					break;
				}
			}
		}
		//Opção 0 - Criar Conta
		else if (strcmp(buff, "0\n") == 0) {
			printf("Nome de usuario: ");
			bzero(buff, sizeof(buff));
			bzero(name, sizeof(name));
			n = 0;
			//Lendo username
			while ((buff[n++] = getchar()) != '\n')
				;

			//Enviando o username para o servidor
			strncpy(name, buff, n-1);
			send(sockfd, name, sizeof(name), FLAGS);
			recv(sockfd, &retval, sizeof(int), FLAGS);
			//Tratando erros
			if (retval == -1) {
				printf("\nConta ja existe, tente de novo.\n");
			}
			else {
				printf("\nConta criada. Faca Login para continuar\n");
			}
		}
		//Opção 7 - Sair do Programa
		else if (strcmp(buff, "7\n") == 0) {
			printf("Fechando Cliente...\n");
			break;
		}
		else {
			printf("Opcao invalida. Tente de novo\n");
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Usage:\n\t%s [ServerIP] [Port Number]\n", argv[0]);
		exit(1);
	}
	int sockfd;
	struct sockaddr_in servaddr;

	//Criando socket TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//Abortando o programa se o socket nao foi criado corretamente
	if (sockfd == -1) {
		printf("Error creating socket\n");
		exit(1);
	}

	//Definindo o endereco e port recebidos na linha de comando
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	//Conectando com o servidor
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection with the server failed...\n");
		exit(1);
	}

	//Funcao que trata a comunicacao entre esse cliente e o servidor
	func(sockfd);

	//Fechando o socket apos a conexao
	close(sockfd);

	return 0;
}