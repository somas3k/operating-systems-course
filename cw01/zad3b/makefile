LDIR=../zad1/lib
SDIR=../zad2/
IDIR=../zad1/includes/
CC=gcc
CFLAGS=-std=gnu99 -Wall
ST=STATIC_LIBRARY:
SH=SHARED_LIBRARY:
DSH=DYNAMIC_SHARED_LIBRARY:

all:
	make run_with_opt OPT=-O0
	make run_with_opt OPT=-O1
	make run_with_opt OPT=-Os
	make run_with_opt OPT=-O2
	make run_with_opt OPT=-O3
	make run_with_opt OPT=-Ofast

run_with_opt: check_lib with_static_lib with_shared_lib with_dyn_sh_lib

check_lib:
	cd ../zad1/ && make

with_static_lib: check_lib $(SDIR)main.c
	$(CC) $(CFLAGS) $(OPT) $(SDIR)main.c -o progwithstlib -I$(IDIR) -L$(LDIR) -lcntbookst
	echo $(ST) >> result.txt
	echo with optimalization $(OPT) >>result.txt
	./progwithstlib >> result.txt

with_shared_lib: check_lib $(SDIR)main.c
	$(CC) $(CFLAGS) $(OPT) $(SDIR)main.c -o progwithshlib -I$(IDIR) -L$(LDIR) -lcntbooksh -Wl,-rpath,$(LDIR)
	echo $(SH) >> result.txt
	echo with optimalization $(OPT) >> result.txt
	./progwithshlib >> result.txt

with_dyn_sh_lib: $(SDIR)main2.c
	$(CC) $(CFLAGS) $(OPT) -ldl $(SDIR)main2.c -o progwithdynshlib -I$(IDIR)
	echo $(DSH) >> result.txt
	echo with optimalization $(OPT) >> result.txt
	./progwithdynshlib >> result.txt

clean:
	cd ../zad1/ && make clean
	rm -f result.txt progwithstlib progwithshlib
