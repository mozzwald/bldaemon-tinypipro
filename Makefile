# Makefile for bldaemon-tinypipro
CC?=gcc
CFLAGS=-L. -L.. -O2 --std=c99
LIBS=-lrt -lpthread -lbcm2835
PREFIX=/usr

all:
	$(CC) $(CFLAGS) -c *.c
	$(CC) $(CFLAGS) *.o -o bldaemon $(LIBS)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)/etc/systemd/system
	cp bldaemon $(DESTDIR)$(PREFIX)/sbin/bldaemon
	cp bldaemon.service $(DESTDIR)/etc/systemd/system/bldaemon.service
	systemctl enable bldaemon.service
	systemctl start bldaemon.service

uninstall:
	systemctl stop bldaemon.service
	systemctl disable bldaemon.service
	rm -f $(DESTDIR)/etc/systemd/system/bldaemon.service
	rm -f bldaemon $(DESTDIR)$(PREFIX)/sbin/bldaemon

clean:
	rm -f *.o bldaemon
