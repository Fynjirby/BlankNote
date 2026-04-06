# Makefile of BlankNote

VERSION = 1.0
CFLAGS += -DVERSION=\"$(VERSION)\"

TARGET = blanknote
PREFIX ?= /usr
DESTDIR ?= 
MANDIR = $(PREFIX)/share/man
ICONDIR = $(PREFIX)/share/icons/hicolor/256x256/apps
DESKTOPDIR = $(PREFIX)/share/applications
PLUGINSDIR = $(PREFIX)/share/$(TARGET)/plugins

build:
	gcc main.c config.c files.c plugins.c -o $(TARGET) $(shell pkg-config --cflags --libs gtk4) $(CFLAGS)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)

install:
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	sed "s/VERSION/$(VERSION)/g" assets/$(TARGET).1 > $(TARGET).1.gen
	sed "s/VERSION/$(VERSION)/g" assets/$(TARGET).5 > $(TARGET).5.gen
	install -Dm644 $(TARGET).1.gen $(DESTDIR)$(MANDIR)/man1/$(TARGET).1
	install -Dm644 $(TARGET).5.gen $(DESTDIR)$(MANDIR)/man5/$(TARGET).5
	rm -f $(TARGET).1.gen $(TARGET).5.gen
	install -Dm644 assets/$(TARGET).desktop $(DESTDIR)$(DESKTOPDIR)/blanknote.desktop
	install -Dm644 assets/$(TARGET).png $(DESTDIR)$(ICONDIR)/blanknote.png
	install -d $(DESTDIR)$(PLUGINSDIR)
	test -d plugins && cp -f plugins/* $(DESTDIR)$(PLUGINSDIR) || true

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -f $(DESTDIR)$(MANDIR)/man1/$(TARGET).1
	rm -f $(DESTDIR)$(MANDIR)/man5/$(TARGET).5
	rm -f $(DESTDIR)$(DESKTOPDIR)/$(TARGET).desktop
	rm -f $(DESTDIR)$(ICONDIR)/$(TARGET).png
	rm -fr $(DESTDIR)$(PREFIX)/share/$(TARGET)

.PHONY: build run clean install uninstall
