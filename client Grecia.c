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
#define MAX 80
#define PORT 8080
#define FLAGS 0
#define SA struct sockaddr

void send_file(FILE *fp, int sockfd){
  int n;
  char data[MAX] = {0};

  while(fgets(data, MAX, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, MAX);
  }
}

void func(int sockfd)
{
	char buff[MAX];
	char name[MAX];
	int n;
	int retval;
	FILE *fp;
	for(;;){
	printf("Criar(0), Logar(1) ou Sair(7)?\n");
	bzero(buff, sizeof(buff));
	n = 0;
	while ((buff[n++] = getchar()) != '\n')
		;
	write(sockfd, buff, sizeof(buff));
	//Opção 1 - Fazer Login
	if (strncmp(buff, "1", 1) == 0) {
		printf("Logging\n");
		for(;;){
		//Send Username
		bzero(buff, sizeof(buff));
		bzero(name, sizeof(name));
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		strncpy(name, buff, n-1);
		write(sockfd, name, sizeof(name));
		recv(sockfd, &retval, sizeof(int), FLAGS);
		if (retval != -1) {
			printf("Logado\n\n");
				for(;;){
				printf("Escolha uma opção:\n");
				printf("1. List\n");
				printf("2. Send\n");
				printf("3. Copy\n");
				printf("4. RemoveFile\n");
				printf("5. RemoveAll\n");
				printf("6. RemoveAcc\n");
				printf("7. Exit\n");
				//Send Option
				bzero(buff, sizeof(buff));
				n = 0;
				while ((buff[n++] = getchar()) != '\n')
					;
				write(sockfd, buff, sizeof(buff));
				//Option Selector 1 - List Files
				if (strncmp(buff, "1", 1) == 0){
					read(sockfd, buff, sizeof(buff));
					while(strncmp(buff, "-1", 2) != 0){
						printf("%s\n", buff);
						bzero(buff, sizeof(buff));
						read(sockfd, buff, sizeof(buff));
					}
				}
				//Option Selector 2 - Send File to Server
				else if (strncmp(buff, "2", 1) == 0) {
					//sending file
					printf("Sending\n");
					bzero(name, sizeof(name));
					bzero(buff, sizeof(buff));
					n = 0;
					while ((buff[n++] = getchar()) != '\n')
						;
					printf("File Name = %s\n", buff);
					strncpy(name, buff, n-1);
					fp = fopen(name, "r");
					//send file name
					write(sockfd, name, sizeof(name));
					if (fp == NULL) {
						perror("[-]Error in reading file.");
						exit(1);
					}
					//send file
					send_file(fp, sockfd);
					fclose(fp);
					printf("Sent File\n");
				}
				//Option Selector 3 - Copy File from Server
				else if (strncmp(buff, "3", 1) == 0) {
					printf("Sending File_Name\n");
					bzero(name, sizeof(name));
					bzero(buff, sizeof(buff));
					n = 0;
					while ((buff[n++] = getchar()) != '\n')
						;
					strncpy(name, buff, n-1);
					printf("File Name = %s\n", name);
					write(sockfd, name, sizeof(name));
					fp = fopen(name, "w");
					printf("Receiving\n");
					read(sockfd, buff, sizeof(buff));
					fprintf(fp, "%s", buff);
	  				fclose(fp);
					printf("Done\n");
				}
				//Option Selector 4 - Delete Single File
				else if (strncmp(buff, "4", 1) == 0) {
					bzero(name, sizeof(name));
					bzero(buff, sizeof(buff));
					n = 0;
					while ((buff[n++] = getchar()) != '\n')
						;
					strncpy(name, buff, n-1);
					printf("File Name = %s\n", name);
					write(sockfd, name, sizeof(name));
				}
				//Option Selector 5 - Delete All Files
				else if (strncmp(buff, "5", 1) == 0) {
					printf("Deleting All Files\n");
				}
				//Option Selector 6 - Delete Account + Logout
				else if (strncmp(buff, "6", 1) == 0) {
					printf("Deleting Account\n");
					break;
				}
				//Option Selector 7 - Logout
				else if (strncmp(buff, "7", 1) == 0) {
					printf("Logout...\n");
					break;
				}
				else {
					printf("Option %s not found\n", buff);
				}
			}
			break;
	}
		else{
			printf("Conta inexistente\n");
		}
	}
	}
	//Opção 0 - Criar Conta
	else if (strncmp(buff, "0", 1) == 0) {
		printf("Creating\n");
		bzero(buff, sizeof(buff));
		bzero(name, sizeof(name));
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		strncpy(name, buff, n-1);
		write(sockfd, name, sizeof(name));
		recv(sockfd, &retval, sizeof(int), FLAGS);
		if (retval == -1) {
			printf("Account already exists.\n");
		}
		else{
			printf("Account Created, Size=%ld.\n", strlen(buff)-1);
		}
	}
	//Opção 7 - Sair do Programa
	else if (strncmp(buff, "7", 1) == 0) {
		printf("Closing Client...\n");
		break;
	}
}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}