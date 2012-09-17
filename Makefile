CC     = gcc -std=c99
DEBUG  = -Wall -g

CFLAGS = `pkg-config --cflags --libs glib-2.0` -lm
GFLAGS = `pkg-config --cflags --libs gtk+-3.0 gmodule-export-2.0` -lm

COMMON = -Isrc/main/ src/main/matrix.c src/main/utils.c src/main/latex.c src/main/graphviz.c

all: bin/main bin/floyd bin/knapsack
#bin/probwin bin/optbst bin/replacement

test: bin/test/floyd bin/test/knapsack bin/test/probwin

# Main binaries
bin/main: src/main/main.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(GFLAGS)

bin/floyd: src/floyd/main.c src/floyd/floyd.c src/floyd/report.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(GFLAGS)

bin/knapsack: src/knapsack/main.c src/knapsack/knapsack.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(GFLAGS)

# Test binaries
bin/test/floyd: src/floyd/test.c src/floyd/floyd.c src/floyd/report.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(CFLAGS)

bin/test/knapsack: src/knapsack/test.c src/knapsack/knapsack.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(CFLAGS)

bin/test/probwin: src/probwin/test.c src/probwin/probwin.c
	$(CC) $(DEBUG) -o $@ $? $(COMMON) $(CFLAGS)

# Clean
clean:
	rm -f `find bin/ -executable -type f`
	rm -f `find bin/test/ -executable -type f`
