CFLAGS=-Wall -std=c99

all: prog interpreter 

prog: prog.c
	gcc $(CFLAGS) -o prog prog.c

interpreter: main.c
	gcc $(CFLAGS) -o interpreter main.c

clean:	
	rm -f prog interpreter

.PHONY: all prog clean interpreter run
