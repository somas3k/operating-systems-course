CFLAGS = -Wall -lm


compile: master slave test1

master: master.c
	gcc $(CFLAGS) -o master master.c

slave: slave.c
	gcc $(CFLAGS) -o slave slave.c

test1: test.c
	gcc -Wall -o test test.c

clean:
	rm -f master slave data
