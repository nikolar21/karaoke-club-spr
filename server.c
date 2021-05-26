#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/stat.h>  
#include <fcntl.h>
#define MAX 3000
#define PORT 8087
#define SA struct sockaddr

typedef struct Request {
   char title[50];
   char artist[50];
}Request;

typedef struct Response {
   char title[50];
   char artist[50];
}Response;  

Response songs[100];

void writeSong(Request newSong) {
	int fd;
	fd = open("songs.bin", O_CREAT|O_APPEND|O_WRONLY);

	if (fd == -1) { /* In the case of error, open returns -1 ! */
    printf("Error: cannot open file songs.bin\n");
    exit(1);
  	}
	//printf("writing song to file");
	write(fd, &newSong, sizeof(Request));

	close(fd);
}

void readSongs(Request requestedSong, int sockFd) {

	char* songFoundMessage = "The song is played for karaoke...";
	char* songNotFoundMessage = "The ordered song is not in my catalog, please order another song";

  int fd;
  fd = open("songs.bin", O_RDONLY);
  
  if (fd == -1){
    printf("Error: file out.bin cannot be opened\n");
    exit(1);
  	}	

  Response savedSongs[15];

  int count = 0;
  
  while(read(fd, &savedSongs[count], sizeof(Response)) == sizeof(Response)) {
   count++;
 }

	int flag = 0;
  int i;
  for (i=0;i<sizeof(savedSongs)/sizeof(Response);i++) {
	  printf("%s", savedSongs[i].title);

	  if (strcmp(requestedSong.title, savedSongs[i].title) == 0 && strcmp(requestedSong.artist, savedSongs[i].artist) == 0) {
		  flag = 1;
		  break;
	  } else {
		  continue;
	  }
  }

  //printf("%s", requestedSong.title);

  if (flag == 1) {
		  write(sockFd, songFoundMessage, 150);
	  } else {
		  write(sockFd, songNotFoundMessage, 150);
		  writeSong(requestedSong);
	  }

	flag=0;

  close(fd);
}

void func(int sockfd)
{
	char buff[MAX];

	for (;;) {

		bzero(buff, MAX);

		Request* request = malloc(sizeof(Request));

		read(sockfd, buff, sizeof(buff));

		memcpy(request, buff, sizeof(Request));

		printf("From client: [%s %s]\n", request->title, request->artist);

		bzero(buff, MAX);
		
		Response* response = malloc(sizeof(Response));

		readSongs(*request, sockfd);

		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("Server acccept failed...\n");
		exit(0);
	}
	else
		printf("Server acccept the client...\n");

	func(connfd);

	close(sockfd);
}
