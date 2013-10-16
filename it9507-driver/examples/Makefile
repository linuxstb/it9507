CC = gcc
CFLAGS = -W -Wall -D_FILE_OFFSET_BITS=64

all: sendts gain_range

sendts.o: sendts.c ../include/dvbmod.h
	$(CC) $(CFLAGS) -o sendts.o -c sendts.c

sendts: sendts.o
	$(CC) -o sendts sendts.o

gain_range.o: gain_range.c ../include/dvbmod.h
	$(CC) $(CFLAGS) -o gain_range.o -c gain_range.c

gain_range: gain_range.o
	$(CC) -o gain_range gain_range.o

clean:
	rm -f sendts sendts.o gain_range gain_range.o *~
