CFLAGS = -Wall -std=c99

all: interpreter prog

interpreter: main.c
	gcc $(CFLAGS) -o interpreter main.c

prog: prog.c
	gcc $(CFLAGS) -o prog prog.c

clean:
	rm -f interpreter prog
