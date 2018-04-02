CFLAGS= -Wall -std=gnu99 -pthread -lrt

all: clients barber

clients: clients.c FIFO.c FIFO.h helpers.c helpers.h
	gcc $(CFLAGS) -o clients clients.c FIFO.c helpers.c

barber: barber.c FIFO.c FIFO.h helpers.c helpers.h
	gcc $(CFLAGS) -o barber barber.c FIFO.c helpers.c

clean:
	rm -f clients barber
