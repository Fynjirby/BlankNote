# Makefile of BlankNote

VERSION = 1.0.1

DESTDIR = /usr/local/bin
MANDIR = /usr/local/share/man/man1
ICONDIR = $(HOME)/.local/share/icons/hicolor/256x256/apps
DESKTOPDIR = $(HOME)/.local/share/applications

build:
	gcc main.c -o blanknote $(shell pkg-config --cflags --libs gtk4)

clean:
	rm -f blanknote

install:
	mkdir -p $(DESTDIR)
	cp -f blanknote $(DESTDIR)
	chmod +x $(DESTDIR)/blanknote
	mkdir -p $(MANDIR)
	sed "s/VERSION/$(VERSION)/g" < assets/blanknote.1 > $(MANDIR)/blanknote.1
	chmod 644 $(MANDIR)/blanknote.1

uninstall:
	rm -f $(DESTDIR)/blanknote
	rm -f $(MANDIR)/blanknote.1

reinstall:
	$(MAKE) clean
	$(MAKE)
	sudo $(MAKE) install

desktop:
	mkdir -p $(ICONDIR)
	cp -f assets/blanknote.png $(ICONDIR)
	chmod 644 $(ICONDIR)/blanknote.png
	mkdir -p $(DESKTOPDIR)
	cp -f assets/blanknote.desktop $(DESKTOPDIR)
	chmod 644 $(DESKTOPDIR)/blanknote.desktop

.PHONY: build clean install uninstall reinstall desktop
