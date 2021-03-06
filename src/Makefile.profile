CC=gcc
CFLAGS=-Wall -Wextra -Werror -O3 -pg -g -I ../include/
#Doing the -rpath . here simply so someone doesn't have to move the libtcod library into their /usr/lib if they want.
LDFLAGS=-Wl,-rpath . -ltcod_x64 -pg -g -L ../lib/
SOURCES=monster.c map.c cave-walk.c draw.c game.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=../bin/testrl

all: $(EXECUTABLE)
	@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJECTS)

superclean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
