CC=gcc
CFLAGS=-g -Wall -Wextra -std=gnu99
all: plexio

list_test: list_test.o list.o
lines_test: lines_test.o lines.o

clean:
	rm -f *.o plexio lines_test list_test

plexio: plexio.o child.o io.o list.o lines.o

plexio.o: plexio.c plexio.h child.h io.h list.h lines.h
child.o: child.c child.h plexio.h
io.o: io.c io.h
list.o: list.c list.h
lines.o: lines.c lines.h
list_test.o: list_test.c list.h
lines_test.o: lines_test.c lines.h
