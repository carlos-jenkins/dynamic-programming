CC     = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wl,--export-dynamic
LIBS   = `pkg-config --libs gtk+-3.0 gmodule-export-2.0`
DEBUG  = -Wall -g
OUTPUT = bin/

OBJECTS = main.o

all: main

main: $(OBJECTS)
	$(CC) $(DEBUG) $(OBJECTS) -o $(OUTPUT)$@ $(LIBS)

main.o: src/main/main.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o main
