LIBS=`sdl-config --libs` -lSDL_net
CFLAGS=`sdl-config --cflags` -Wall -I./include -g
CC=g++

all: ditchs

clean:  
	rm -f ditchs obj/src/*.o

obj/src/main.o:
	$(CC) $(CFLAGS) -o obj/src/main.o -c src/main.cpp

src: obj/src/main.o

ditchs: src
	$(CC) -g -o ditchs $(LIBS) obj/src/*.o

