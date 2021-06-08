#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#define MAX 80
#define PORT 8080
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

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	int n, z;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);
		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("From client: %s\n", buff);
		if (strncmp("1", buff, 1) == 0) {
			write(sockfd, "Logando", 7);
			bzero(buff, MAX);
			read(sockfd, buff, sizeof(buff));
			write(sockfd, "Valid", 5);
			for(;;){
				read(sockfd, buff, sizeof(buff));
				if (strncmp("1", buff, 1) == 0) {
					d = opendir(".");
					if (d){
					    while ((dir = readdir(d)) != NULL){
					    	printf("%s\n", dir->d_name);
					    	strcpy(buff,dir->d_name);
				        	write(sockfd, buff, sizeof(buff));
				        	bzero(buff, MAX);
				    	}
				    	closedir(d);
					}
					write(sockfd, "-1", 2);
					printf("Exit Loop\n");
				}
				else if (strncmp("2", buff, 1) == 0) {
					printf("Receiving\n");
					read(sockfd, buff, sizeof(buff));
					fp = fopen(buff, "w");
					bzero(buff, MAX);
					read(sockfd, buff, sizeof(buff));
					fprintf(fp, "%s", buff);
	    			bzero(buff, MAX);
	  				fclose(fp);
					printf("Done\n");
				}
				else if (strncmp("3", buff, 1) == 0) {
					printf("Copying\n");
					bzero(buff, sizeof(buff));
					read(sockfd, buff, sizeof(buff));
					fp = fopen(buff, "r");
					if (fp == NULL) {
						perror("[-]Error in reading file.");
						exit(1);
					}
					send_file(fp, sockfd);
					fclose(fp);
					printf("Done\n");
				}
				else if (strncmp("4", buff, 1) == 0) {
					printf("Removing Single File\n");
					read(sockfd, buff, sizeof(buff));
					if (remove(buff) == 0)
						printf("Deleted successfully\n");
					else
						printf("Unable to delete the file\n");
				}
				else if (strncmp("5", buff, 1) == 0) {
					printf("Remove All Files\n");
					d = opendir(".");
					if (d){
					    while ((dir = readdir(d)) != NULL){
					    	printf("%s\n", dir->d_name);
					    	scanf("%d", &z);
					    	if (z==1){
					    		printf("Deleting\n");
					    		remove(dir->d_name);
					    	}
					    	else
					    		printf("Keeping\n");
				    	}
				    	closedir(d);
					}
					printf("Done\n");
				}
				else if (strncmp("6", buff, 1) == 0) {
					printf("Remove Acc\n");
				}
				else if (strncmp("7", buff, 1) == 0) {
					printf("Server Exit...\n");
				break;
				}
				else {
					printf("Cmd Not Found\n");
				}
			}
		}
		else if (strncmp("0", buff, 1) == 0) {
			write(sockfd, "Criando", 7);
		}
		else if (strncmp("7", buff, 1) == 0) {
			printf("Create/Log Exit...\n");
			write(sockfd, "Saindo", 6);
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