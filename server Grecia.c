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
/*
void write_file(int sockfd, char* buff){
  int n;
  FILE *fp;
  char buffer[MAX];

  fp = fopen(buff, "w");
  while (1) {
    n = recv(sockfd, buffer, MAX, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, MAX);
  }
  fclose(fp);
  return;
}
*/

// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	int n;
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
				        	write(sockfd, dir->d_name, 30);
				    	}
				    	closedir(d);
					}
					write(sockfd, "-1", 2);
					printf("Exit Loop\n");
				}
				else if (strncmp("2", buff, 1) == 0) {
					printf("Sending\n");
					read(sockfd, buff, sizeof(buff));
					fp = fopen(buff, "w");
					bzero(buff, MAX);
					read(sockfd, buff, sizeof(buff));
					fprintf(fp, "%s", buff);
	    			bzero(buff, MAX);
	  				fclose(fp);
					printf("Sent\n");
				}
				else if (strncmp("3", buff, 1) == 0) {
					printf("Copy\n");
				}
				else if (strncmp("4", buff, 1) == 0) {
					printf("Removing Single File\n");
					read(sockfd, buff, sizeof(buff));
					if (remove(buff) == 0)
						printf("Deleted successfully");
					else
						printf("Unable to delete the file");
					printf("Removed\n");
				}
				else if (strncmp("5", buff, 1) == 0) {
					printf("Remove All Files\n");
				}
				else if (strncmp("6", buff, 1) == 0) {
					printf("Remove Acc\n");
				}
				else if (strncmp("7", buff, 1) == 0) {
					printf("Server Exit...\n");
				break;
				}
				else {
					printf("Test\n");
					bzero(buff, sizeof(buff));
					n = 0;
					while ((buff[n++] = getchar()) != '\n')
						;
					write(sockfd, buff, sizeof(buff));
				}
			}
		}
		else if (strncmp("0", buff, 1) == 0) {
			write(sockfd, "Criando", 7);
		}
		else if (strncmp("7", buff, 1) == 0) {
			printf("Create/Log Exit...\n");
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