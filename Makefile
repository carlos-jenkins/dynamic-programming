CC     = gcc
DEBUG  = -Wall -g

CFLAGS = `pkg-config --cflags --libs glib-2.0`
GFLAGS = `pkg-config --cflags --libs gtk+-3.0 gmodule-export-2.0`

COMMON =

all: bin/main 
#bin/floyd bin/knapsack bin/optbst bin/probwin bin/replacement

bin/main: src/main/main.c
	$(CC) $(DEBUG) -o $@ $< $(COMMON) $(GFLAGS)


clean:
	rm -f bin/*

