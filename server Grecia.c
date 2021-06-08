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

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	char path[20];
	char file_path[40];
	FILE *fp;
	DIR *d, *p;
	struct dirent *dir;
	int n, retval;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);
		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\n", buff);
		// 1 - Fazer Login
		if (strncmp("1", buff, 1) == 0) {
			bzero(buff, MAX);
			bzero(path, 20);
			bzero(file_path, 40);
			strcat(path,"db/");
			read(sockfd, buff, sizeof(buff));
			strcat(path, buff);
			p = opendir(path);
			if (p){
				printf("Opened Directory\n");
				write(sockfd, buff, sizeof(buff)); 
				retval=strlen(buff);
				send(sockfd, &retval, sizeof(int), FLAGS);
					for(;;){
					//Option Selector
					read(sockfd, buff, sizeof(buff));
					//Option Selector 1 - List Files
					if (strncmp("1", buff, 1) == 0) {
						d = opendir(path);
					    while ((dir = readdir(d)) != NULL){
					    	printf("%s\n", dir->d_name);
					    	strcpy(buff,dir->d_name);
				        	write(sockfd, buff, sizeof(buff));
				        	bzero(buff, MAX);
				    	}
				    	closedir(d);
						write(sockfd, "-1", 2);
						printf("Exit Loop\n");
					}
					//Option Selector 2 - Receive File from Client
					else if (strncmp("2", buff, 1) == 0) {
						printf("Receiving\n");
						read(sockfd, buff, sizeof(buff));
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
						printf("Done\n");
					}
					//Option Selector 3 - Send File to Client
					else if (strncmp("3", buff, 1) == 0) {
						printf("Copying\n");
						bzero(buff, sizeof(buff));
						read(sockfd, buff, sizeof(buff));
						bzero(file_path, 40);
						strcat(file_path, path);
						strcat(file_path, "/");
						strcat(file_path, buff);
						fp = fopen(file_path, "r");
						if (fp == NULL) {
							perror("[-]Error in reading file.");
							exit(1);
						}
						send_file(fp, sockfd);
						fclose(fp);
						printf("Done\n");
					}
					//Option Selector 4 - Remove Single File
					else if (strncmp("4", buff, 1) == 0) {
						printf("Removing Single File\n");
						read(sockfd, buff, sizeof(buff));
						bzero(file_path, 40);
						strcat(file_path, path);
						strcat(file_path, "/");
						strcat(file_path, buff);
						if (remove(file_path) == 0)
							printf("Deleted successfully\n");
						else
							printf("Unable to delete the file\n");
					}
					//Option Selector 5 - Remove All Files
					else if (strncmp("5", buff, 1) == 0) {
						printf("Remove All Files\n");
						d = opendir(path);
					    while ((dir = readdir(d)) != NULL){
					    	bzero(file_path, 40);
							strcat(file_path, path);
							strcat(file_path, "/");
							strcat(file_path, dir->d_name);
					    	remove(file_path);
				    	}
				    	closedir(d);
						printf("Done\n");
					}
					//Option Selector 6 - Remove Account
					else if (strncmp("6", buff, 1) == 0) {
						printf("Remove Acc\n");
						strcat(file_path, path);
						strcat(file_path, "/");
						remove(file_path);
						break;
					}
					//Option Selector 7 - Logout
					else if (strncmp("7", buff, 1) == 0) {
						printf("Logout...\n");
					break;
					}
					else {
						printf("Cmd Not Found\n");
					}
				}
		}
			else{
				printf("Failed to Open Directory\n");
			}
			closedir(p);
		}
		// 0 - Criar Conta
		else if (strncmp("0", buff, 1) == 0) {
			read(sockfd, buff, sizeof(buff));
			retval = create_account(buff);
            send(sockfd, &retval, sizeof(int), FLAGS);
		}
		// 7 - Fechar Programa
		else if (strncmp("7", buff, 1) == 0) {
			printf("Closing Server...\n");
			break;
		}
		else{
			printf("Cmd Not Understood");
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		printf("server acccept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}