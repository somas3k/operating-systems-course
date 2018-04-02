LDIR=../zad1/lib
SDIR=../zad2/
IDIR=../zad1/includes/
CC=gcc
CFLAGS=-std=gnu99 -Wall -g

all: check_lib with_static_lib


check_lib:
	cd ../zad1/ && make

with_static_lib: main.c 
	$(CC) $(CFLAGS) main.c -o progwithstlib -I$(IDIR) -L$(LDIR) -lcntbookst
	
clean:
	rm -f progwithstlib
