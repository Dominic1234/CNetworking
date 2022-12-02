#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORTNUM 2300

int main(int argc, char *argv[]) {
	char* msg = "Hello, World\n";

	struct sockaddr_in dest;			/* socket info about client */
	struct sockaddr_in serv;			/* socket info about server */
	int mysocket;					/* socket used to listen for incoming connections */
	socklen_t socksize = sizeof(struct sockaddr_in);
	pid_t childpid;

	memset(&serv, 0, sizeof(serv));			/* empty struct before filling fields */
	serv.sin_family = AF_INET;			/* set connection type to TCP/IP */
	serv.sin_addr.s_addr = htonl(INADDR_ANY);	/* set server address to any interface */
	serv.sin_port = htons(PORTNUM);			/* set server port number */

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mysocket < 0) {
		printf("Error in connection\n");
		exit(1);
	}

	printf("Server socket created\n");

	/* bind server info to mysocket*/
	int ret = bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

	if(ret < 0) {
		printf("Ërror in binding\n");
		exit(1);
	}

	/* start listening, allowing a queue of 1 pending connection */
	if(listen(mysocket, 10) == 0) {
		printf("Listening...\n\n");
	}

	int consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);

	if(consocket < 0) {
		exit(1);
	}

	while(consocket) {
		printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));
		send(consocket, msg, strlen(msg), 0);
		consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
	}

	close(consocket);
	close(mysocket);
	return EXIT_SUCCESS;
}
