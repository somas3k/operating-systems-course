CFLAGS= -Wall -std=gnu99 -pthread

all: server client

server: server.c common.h
	gcc $(CFLAGS) server.c -o server

client: client.c common.h
	gcc $(CFLAGS) client.c -o client

clean: 
	rm -f server client
