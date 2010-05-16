LIBS=`sdl-config --libs` -lSDL_net
CFLAGS=`sdl-config --cflags` -Wall -I./include -g
CC=g++

all: ditchs

ditchs: src
	$(CC) -g -o ditchs $(LIBS) obj/src/*.o

obj/src/main.o:
	@mkdir -p obj/src
	$(CC) $(CFLAGS) -o obj/src/main.o -c src/main.cpp

src: obj/src/main.o

clean:  
	rm -f ditchs
	rm -rf obj
