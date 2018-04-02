CC= gcc
CFLAGS=-std=gnu99 -Wall
IDIR=./includes/
LDIR=./lib/
ODIR=./obj/
SDIR=./src/

all: libcntbookst.a libcntbooksh.so

libcntbookst.a: cntbkonlist.o cntbkontree.o contact.o
	ar -cr $(LDIR)libcntbookst.a $(ODIR)cntbkonlist.o $(ODIR)cntbkontree.o $(ODIR)contact.o

libcntbooksh.so: cntbkonlist.o cntbkontree.o contact.o
	$(CC) $(CFLAGS) -fPIC -shared -o $(LDIR)libcntbooksh.so $(ODIR)cntbkonlist.o $(ODIR)cntbkontree.o $(ODIR)contact.o

contact.o: $(SDIR)contact.c $(IDIR)contact.h
	$(CC) $(CFLAGS) -fPIC -c $(SDIR)contact.c -o  $(ODIR)contact.o

cntbkontree.o: $(SDIR)cntbkontree.c $(IDIR)cntbkontree.h
	$(CC) $(CFLAGS) -fPIC -c $(SDIR)cntbkontree.c -o  $(ODIR)cntbkontree.o

cntbkonlist.o: $(SDIR)cntbkonlist.c $(IDIR)cntbkonlist.h
	$(CC) $(CLAGS) -fPIC -c $(SDIR)cntbkonlist.c -o  $(ODIR)cntbkonlist.o

clean:
	rm -f $(ODIR)*.o $(LDIR)*.so $(LDIR)*.a



