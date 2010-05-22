LUACFLAGS = `pkg-config lua5.1 --cflags`
LUALIBS   = `pkg-config lua5.1 --libs`

SDLCFLAGS = `sdl-config --cflags`
SDLLIBS   = `sdl-config --libs`

CC        = g++

CFLAGS    = -Wall -g
MYCFLAGS  = -I./include $(SDLCFLAGS) $(LUACFLAGS) 

LDFLAGS   = -g
MYLDFLAGS =
LIBS      = $(SDLLIBS) -lSDL_image -lSDL_gfx -lSDL_net -lguichan -lguichan_sdl $(LUALIBS) -lboost_filesystem -lpng -ltinyxml

CPPFILES    = $(shell find src -name '*.cpp' | sort)
OBJECTFILES = $(shell echo $(CPPFILES) | sed ": a;s:src/:obj/:;s:.cpp:.o:;t a")
TARGET      = ditcher

all: $(TARGET)

$(TARGET): $(OBJECTFILES)
	$(CC) $(LDFLAGS) $(MYLDFLAGS) -o $(TARGET) $(OBJECTFILES) $(LIBS)

obj/%.o: src/%.cpp
	@mkdir -p `dirname $@`
	$(CC) -c $< -o $@ $(CFLAGS) $(MYCFLAGS)

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

