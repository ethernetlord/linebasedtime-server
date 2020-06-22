CC=gcc
CFLAGS=-Wall
SHELL=/bin/sh
TARGETS=linebasedtime_server
DESTDIR=/usr/local/bin

.PHONY: all install clean systemd-unit



all: $(TARGETS)

linebasedtime_server: linebasedtime_server.c
		$(CC) $(CFLAGS) -o$@ $^

install:
		strip $(TARGETS) && cp $(TARGETS) $(DESTDIR)/lbtimed

clean:
		rm $(TARGETS)

systemd-unit:
		cp lbtimed.service /lib/systemd/system/lbtimed.service && systemctl daemon-reload
