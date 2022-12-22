#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXRCVLEN 500
#define PORTNUM 2300

// Semaphore variables
sem_t x, y;
pthread_t tid;
pthread_t threads[100];
int count = 0;

// Global variables
char* welcome = "Hello, World\n";

typedef struct threadArgs {
	int sock;
	void* size;	
} arg;

void* thread(void* argstmp) {
	arg* args = (arg*)argstmp;
	char msg[MAXRCVLEN+1] = "\0";
	printf("Writer entering\n");
	// Lock semaphore
	sem_wait(&y);
	printf("Writer entered\n");
	//send greeting
	send(*(int*)args->size, welcome, strlen(msg)+1, 0);

	//begin main code after inital handshake completed
	int choice = 3, len;
	while(choice != 3) {
		len = recv(args->sock, &msg, sizeof(int), 0);
		choice = atoi(msg);

		if(choice == 1) {
			send(*(int*)args->size, msg, strlen(msg)+1, 0);
		}
		else if(choice == 2) {
			len = recv(args->sock, &msg, MAXRCVLEN, 0);
			printf("Message received: \"%s\" of size %d\n", msg, len);
		}
		else if(choice == 3) {
			continue;
		}
		else {
			printf("Error, invalid choice: %d\n", choice);
		}
	}
	// Unlock semaphore
	sem_post(&y);
	printf("Writer leaving\n");

	return NULL;
}

int main(int argc, char *argv[]) {

	struct sockaddr_in dest;			/* socket info about client */
	struct sockaddr_in serv;			/* socket info about server */
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);

	int mysocket;					/* socket used to listen for incoming connections */
	
	memset(&serv, 0, sizeof(serv));			/* empty struct before filling fields */
	serv.sin_family = AF_INET;			/* set connection type to TCP/IP */
	serv.sin_addr.s_addr = htonl(INADDR_ANY);	/* set server address to any interface */
	serv.sin_port = htons(PORTNUM);			/* set server port number */

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mysocket < 0) {
		perror("Error in connection\n");
		exit(1);
	}

	printf("Server socket created\n");

	/* bind server info to mysocket*/
	int ret = bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

	if(ret < 0) {
		perror("Ërror in binding\n");
		exit(1);
	}

	/* start listening, allowing a queue of 10 pending connection */
	if(listen(mysocket, 10) == 0) {
		printf("Listening...\n\n");
	}
	else {
		perror("Error\n");
		exit(1);
	}

		int consocket, i = 0;

	while(1) {
		addr_size = sizeof(serverStorage);
		consocket = accept(mysocket, (struct sockaddr *)&dest, &addr_size);
		if(consocket < 0) {
			perror("Error\n");
			exit(1);
		}
		arg argtmp = {mysocket, &consocket};
		if(pthread_create(&threads[i++], NULL, thread, &argtmp) != 0)
			perror("Failed to create thread\n");
	}

	close(mysocket);
	return EXIT_SUCCESS;
}
