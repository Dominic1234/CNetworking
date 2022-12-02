#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAXRCVLEN 500
#define PORTNUM 2300

int main(int argc, char *argv[]) {
	char buffer[MAXRCVLEN + 1];
	int len, mysocket;
	struct sockaddr_in dest;

	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	if(mysocket < 0) {
		printf("Error in connection\n");
		exit(1);
	}
	printf("Client socket is created.\n");

	memset(&dest, 0, sizeof(dest));			/* zero the struct */
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	/* set destination IP number - localhost, 127.0.0.1 */
	dest.sin_port = htons(PORTNUM);			/* set destination port number */

	int ret = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

	if(ret < 0) {
		printf("Error in connection\n");
		exit(1);
	}
	printf("Connected to Server\n");

	len = recv(mysocket, buffer, MAXRCVLEN, 0);

	/* We have to null terminate the received data ourselves */
	buffer[len] = '\0';

	printf("Received %s (%d bytes). \n", buffer, len);

	close(mysocket);

	return EXIT_SUCCESS;
}
