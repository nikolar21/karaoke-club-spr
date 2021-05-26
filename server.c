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

Response songs[100];

/*
 * Function mainly designed to take as parameter requested song that is not in the file and writes it to songs.bin file. 
 * Function is void so it doesn't return any value.
 */
void addSongToCatalog(Request newSong) {
	int fd;
	fd = open("songs.bin", O_CREAT|O_APPEND|O_WRONLY);

	//In the case of error, open returns -1!
	if (fd == -1) { 
    printf("Error: cannot open file songs.bin\n");
    exit(1);
  	}
	printf("Adding song to catalog.\n");
	write(fd, &newSong, sizeof(Request));

	close(fd);
}

/*
 * Function designed for taking request from client and processing it further. 
 * Expected to search and get the requested song from file and sending response to client.
 * If the song is not present in the file, sends appropriate response.
 * After that calls the function addSongToCatalog(Request newSong) and passes the requested song as parameter.
 * Function is void so it doesn't return any value.
 */
void getSongFromCatalog(Request requestedSong, int sockFd) {

	char* songFoundMessage = "The song is played for karaoke...";
	char* songNotFoundMessage = "The ordered song is not in my catalog, please order another song";

  int fd;
  fd = open("songs.bin", O_RDONLY);
  
  if (fd == -1){
    printf("Error: file songs.bin cannot be opened\n");
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

	  if (strcmp(requestedSong.title, savedSongs[i].title) == 0 && strcmp(requestedSong.artist, savedSongs[i].artist) == 0) {
		  flag = 1;
		  break;
	  } else {
		  continue;
	  }
  }

  if (flag == 1) {
		  write(sockFd, songFoundMessage, 150);
	  } else {
		  write(sockFd, songNotFoundMessage, 150);
		  addSongToCatalog(requestedSong);
	  }

	flag=0;

  close(fd);
}

/*
 * Function designed for communication between client and server. 
 * Sends message to client, receives request from client and process it with calling other function like getSongFromCatalog(*request, sockfd)
 * Function is void so it doesn't return any value.
 */
void communicationProcess(int sockfd)
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

		getSongFromCatalog(*request, sockfd);

		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

/*
 * main function that is designed to wait for the client connection.
 */
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

	// Assigning IP and PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socker to given IP
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Server is ready for listening
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accepts the data packet from client
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("Server acccept failed...\n");
		exit(0);
	}
	else
		printf("Server acccept the client...\n");

	communicationProcess(connfd);

	close(sockfd);
}
