CFLAGS = -Wall -std=c99

all: server client

server: server.c common.h
	gcc $(CFLAGS) -o server server.c

client: client.c common.h
	gcc $(CFLAGS) -o client client.c

clean:
	rm -f server client
