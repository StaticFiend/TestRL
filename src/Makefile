CC=gcc
CFLAGS=-Wall -Wextra -Werror -O3
LDFLAGS=-Wl,-rpath . -ltcod
SOURCES=combat.c monster.c map.c draw.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=../bin/testrl

all: $(EXECUTABLE)
	@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -f $(OBJECTS)

superclean:
	$(RM) -f $(OBJECTS) $(EXECUTABLE)
