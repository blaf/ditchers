LIBS=`sdl-config --libs` -lSDL_image -lSDL_gfx -lSDL_net -lguichan -lguichan_sdl -llua5.1 -lboost_filesystem -lpng
CFLAGS=`sdl-config --cflags` -Wall -I./include -g -O3
CC=g++

CPPFILES=$(shell find src -name '*.cpp')
OBJECTFILES=$(shell find src -name '*.cpp' | sed 's/.cpp/.o/' | sed 's:src/:obj/:' | sort)
TARGET=ditcher

all: $(TARGET)

$(TARGET): $(OBJECTFILES)
	$(CC) -g -O3 -o $(TARGET) $(OBJECTFILES) $(LIBS)

obj/%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

print:
	@echo -e "source: $(CPPFILES)\nobject: $(OBJECTFILES)\ntarget: $(TARGET)"

debug: all
	gdb ./ditcher
	
run: all
	./ditcher

cleanrun: rebuild
	./ditcher

rebuild: clean all

clean: cleantarget cleanobj

cleantarget:
	rm -f $(TARGET)

cleanobj:
	rm -f $(OBJECTFILES)

