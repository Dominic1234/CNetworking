
all:		client	server


client:		client.c
		cc -g -Wall -o client client.c

server:		server.c
		cc -g -Wall -pthread -o server server.c
		
clean:
	rm -rf client server
