#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAXRCVLEN 500
#define PORTNUM 2300
#define MAXTHREADS 100

//thread variables
pthread_t tid;
pthread_t threads[MAXTHREADS];

// Global variables
const char* welcome = "Hello, World\n";

void* thread(void* argstmp) {
	int consock = *(int*)argstmp;
	char msg[MAXRCVLEN+1] = "\0";
	printf("Client connected\n");
	//send greeting
	send(consock, welcome, strlen(welcome)+1, 0);

	//begin main code after inital handshake completed
	int choice = 1, len;
	while(choice != 3) {
		len = recv(consock, &choice, sizeof(int), 0);
		if(len < 1) return NULL;

		printf("Choice received: %d\n", choice);
		if(choice == 1) {
			send(consock, msg, strlen(msg)+1, 0);
		}
		else if(choice == 2) {
			len = recv(consock, msg, MAXRCVLEN, 0);
			if(len < 1) return NULL;

			printf("Message received: \"%s\" of size %d\n", msg, len);
		}
		else if(choice == 3) {
			continue;
		}
		else {
			printf("Error, invalid choice: %d\n", choice);
		}
	}
	printf("Writer leaving\n");

	return NULL;
}

int main(int argc, char *argv[]) {

	struct sockaddr_in dest;			/* socket info about client */
	struct sockaddr_in serv;			/* socket info about server */
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;

	int listen_sock;					/* socket used to listen for incoming connections */
	
	memset(&serv, 0, sizeof(serv));			/* empty struct before filling fields */
	serv.sin_family = AF_INET;			/* set connection type to TCP/IP */
	serv.sin_addr.s_addr = htonl(INADDR_ANY);	/* set server address to any interface */
	serv.sin_port = htons(PORTNUM);			/* set server port number */

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if(listen_sock < 0) {
		perror("Error in connection\n");
		exit(1);
	}

	printf("Server socket created\n");

	/* bind server info to listen_sock*/
	int ret = bind(listen_sock, (struct sockaddr *)&serv, sizeof(struct sockaddr));

	if(ret < 0) {
		perror("Ërror in binding\n");
		exit(1);
	}

	/* start listening, allowing a queue of 10 pending connection */
	if(listen(listen_sock, 10) == 0) {
		printf("Listening...\n\n");
	}
	else {
		perror("Error\n");
		exit(1);
	}

		int consocket, i = 0;

	while(1) {
		addr_size = sizeof(serverStorage);
		consocket = accept(listen_sock, (struct sockaddr *)&dest, &addr_size);
		if(consocket < 0) {
			perror("Error\n");
			exit(1);
		}
		if(pthread_create(&threads[i++], NULL, thread, &consocket) != 0)
			perror("Failed to create thread\n");
		if(i >= 100) {
			printf("maximum limit reached for clients\n");
			break;
		}
	}

	close(listen_sock);
	return EXIT_SUCCESS;
}
