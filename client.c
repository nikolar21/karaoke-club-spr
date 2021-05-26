#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define MAX 3000
#define PORT 8083
#define SA struct sockaddr

typedef struct Request {
   char title[50];
   char artist[50];
}Request;

typedef struct Response {
   char title[50];
   char artist[50];
}Response;    

void communicationProcess(int sockfd)
{
	char buff[MAX];

	for (;;) {

		bzero(buff, sizeof(buff));

		Request* request = malloc(sizeof(Request));
		

		printf("Enter title: ");

		scanf("%s", request->title);
		fflush(stdin);

		printf("Enter artist: ");

		scanf("%s", request->artist);
		fflush(stdin);

		memcpy(buff, request, sizeof(Request));

		write(sockfd, buff, sizeof(buff));

		bzero(buff, sizeof(buff));

		read(sockfd, buff, 150);

		printf("From Server : [%s]\n", buff);

		bzero(buff, sizeof(buff));

		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection with the server failed...\n");
		exit(0);
	}
	else
		printf("Connected to the server..\n");
		
	printf("Welcome to karaoke club, you can order your favorite songs for karaoke below!!\n");

	communicationProcess(sockfd);

	close(sockfd);
}
