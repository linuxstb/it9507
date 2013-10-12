CC = gcc
CFLAGS = -W -Wall -D_FILE_OFFSET_BITS=64

all: sendts

sendts.o: sendts.c ../include/dvbmod.h
	$(CC) $(CFLAGS) -o sendts.o -c sendts.c

sendts: sendts.o
	$(CC) -o sendts sendts.o

clean:
	rm -f sendts sendts.o *~
