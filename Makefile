# Makefile for bldaemon-zipit
CC?=gcc
CFLAGS=-L. -L.. -O2 --std=c99 -lrt -lpthread -lpigpio
LIBS=


all:
	$(CC) $(CFLAGS) -c *.c
	$(CC) $(CFLAGS) *.o -o bldaemon $(LIBS)
	
clean:
	rm -f *.o bldaemon
