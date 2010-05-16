ifeq (`pkg-config --exists lua --print-errors`,"")
LUACFLAGS = `pkg-config lua --cflags`
LUALIBS   = `pkg-config lua --libs`
else
LUACFLAGS = `pkg-config lua5.1 --cflags`
LUALIBS   = `pkg-config lua5.1 --libs`
endif

SDLCFLAGS = `sdl-config --cflags`
SDLLIBS   = `sdl-config --libs`

LIBS   = $(SDLLIBS) -lSDL_image -lSDL_gfx -lSDL_net -lguichan -lguichan_sdl $(LUALIBS) -lboost_filesystem -lpng -ltinyxml
CFLAGS = $(SDLCFLAGS) $(LUALIBS) -Wall -I./include -g
CC     = g++

CPPFILES    = $(shell find src -name '*.cpp' | sort)
OBJECTFILES = $(shell echo $(CPPFILES) | sed ": a;s:src/:obj/:;s:.cpp:.o:;t a")
TARGET      = ditcher

all: $(TARGET)

$(TARGET): $(OBJECTFILES)
	$(CC) -g -o $(TARGET) $(OBJECTFILES) $(LIBS)

obj/%.o: src/%.cpp
	@mkdir -p `dirname $@`
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
	rm -rf obj

