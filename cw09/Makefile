CFLAGS=-Wall -std=gnu99 -pthread

all: starving synchronized

starving: r_w_starving_writers.c r_w_starving_writers.h
	gcc $(CFLAGS) -o starving r_w_starving_writers.c r_w_starving_writers.h

synchronized: r_w_sync.c r_w_sync.h fifo.c fifo.h
	gcc $(CFLAGS) -o synchronized r_w_sync.c r_w_sync.h fifo.c fifo.h

clean:
	rm -f starving synchronized
