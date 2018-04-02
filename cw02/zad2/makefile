CFLAGS=-Wall

all: dirent_prog nftw_prog

dirent_prog: ./dirent/main.c
	gcc $(CFLAGS) -o dirent_prog ./dirent/main.c

nftw_prog: ./nftw/main.c
	gcc $(CFLAGS) -o nftw_prog ./nftw/main.c

clean:
	rm -f dirent_prog nftw_prog
