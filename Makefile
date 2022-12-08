
all:		client	server


client:		client.c
		cc -g -o client client.c

server:		server.c
		cc -g -o server server.c -pthread

