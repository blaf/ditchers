# Example of how make install could look like

# For installation
# DESTDIR is for choosing /usr or /usr/local
# DESTDIR is also used when creating a deb package!
DESTDIR=/usr

# DATADIR=games  install to  /usr/share/games/ditchers
# DATADIR=.      install to  /usr/share/ditchers
DATADIR=games

TARGET= ditchers

TARGETS= ditcher/ditcher ditchs/ditchs

all: $(TARGET)

ditchers:
	make -C ditcher
	make -C ditchs

clean:
	make -C ditcher clean
	make -C ditchs clean

#### Instalation ####
install: $(TARGETS)
	mkdir -p '$(DESTDIR)/bin'
	mkdir -p '$(DESTDIR)/share/applications' 
	mkdir -p '$(DESTDIR)/share/pixmaps'
	mkdir -p '$(DESTDIR)/share/man/man6'
	mkdir -p '$(DESTDIR)/share/$(DATADIR)/ditchers'
	install -D -m755 ditcher/ditcher '$(DESTDIR)/bin'
	install -D -m755 ditchs/ditchs   '$(DESTDIR)/bin'
	install -D -m644 ditcher/ditcher.desktop '$(DESTDIR)/share/applications'
	install -D -m644 ditchs/ditchs.desktop   '$(DESTDIR)/share/applications'
	install -D -m644 ditcher/ditcher.png '$(DESTDIR)/share/pixmaps'
	install -D -m644 ditchs/ditchs.png   '$(DESTDIR)/share/pixmaps'
	install -D -m644 ditchers.6.gz   '$(DESTDIR)/share/man/man6'
	cp -r ditcher/data/* '$(DESTDIR)/share/$(DATADIR)/ditchers'

uninstall:
	rm -rf '$(DESTDIR)/share/$(DATADIR)/ditchers'
	rm -f  '$(DESTDIR)/bin/ditcher'
	rm -f  '$(DESTDIR)/bin/ditchs'
	rm -f  '$(DESTDIR)/share/applications/ditcher.desktop'
	rm -f  '$(DESTDIR)/share/applications/ditchs.desktop'
	rm -f  '$(DESTDIR)/share/pixmaps/ditcher.png'
	rm -f  '$(DESTDIR)/share/pixmaps/ditchs.png'
	rm -f  '$(DESTDIR)/share/man/man6/ditchers.6.gz'



