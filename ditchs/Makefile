CC=g++

LDFLAGS   = -g
MYLDFLAGS =
LIBS      = `sdl-config --libs` -lSDL_net

CFLAGS    = -Wall -g
MYCFLAGS  = -I./include `sdl-config --cflags`

all: ditchs

ditchs: src
	$(CC) $(LDFLAGS) $(MYLDFLAGS) -o ditchs $(LIBS) obj/src/*.o

obj/src/main.o:
	@mkdir -p obj/src
	$(CC) $(CFLAGS) $(MYCFLAGS) -o obj/src/main.o -c src/main.cpp

src: obj/src/main.o

clean:  
	rm -f ditchs
	rm -rf obj
