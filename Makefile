CFLAGS += -Wall -g $(shell dpkg-buildflags --get CFLAGS)
LDFLAGS += -g $(shell dpkg-buildflags --get LDFLAGS)

all:
	$(CC) $(CFLAGS) $(LDFLAGS) novena-whack-lcd.c -o novena-whack-lcd

clean:
	rm -f novena-whack-lcd

install:
	mkdir -p $(DESTDIR)/usr/sbin
	mkdir -p $(DESTDIR)/usr/share/man/man1
	cp novena-whack-lcd $(DESTDIR)/usr/sbin/
	cp novena-whack-lcd.1 $(DESTDIR)/usr/share/man/man1
