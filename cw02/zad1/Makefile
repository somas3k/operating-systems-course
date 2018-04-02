CFLAGS = -Wall
RC=10
RS=16

test:
	rm -f wyniki.txt
	make all RC=1000 RS=4
	make all RC=3000 RS=4
	make all RC=1000 RS=512
	make all RC=3000 RS=512
	make all RC=1000 RS=4096
	make all RC=3000 RS=4096
	make all RC=1000 RS=8192
	make all RC=3000 RS=8192

all: generate sort shuffle
	echo "######################################################################" >> wyniki.txt

shuffle: shuffle_sys shuffle_lib

shuffle_sys: 
	cp ./files/file.dat .
	./prog sys shuffle file.dat $(RC) $(RS) >> wyniki.txt

shuffle_lib:
	cp ./files/file.dat . 
	./prog lib shuffle file.dat $(RC) $(RS) >> wyniki.txt

sort: sort_sys sort_lib

sort_sys:
	cp ./files/file.dat . 
	./prog sys sort file.dat $(RC) $(RS) >> wyniki.txt

sort_lib:
	cp ./files/file.dat . 
	./prog lib sort file.dat $(RC) $(RS) >> wyniki.txt


generate: prog
	./prog generate ./files/file.dat $(RC) $(RS)
	
prog: main.c
	gcc $(CFLAGS) -o prog main.c

clean:
	rm -f ./files/file.dat file.dat prog wyniki.txt
	
