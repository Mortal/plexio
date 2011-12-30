CFLAGS=-g -Wall -Wextra -std=c99
all: plexio

list_test: list_test.o list.o

clean:
	rm -f *.o plexio

plexio: plexio.o child.o io.o

plexio.o: plexio.c child.h plexio.h io.h
child.o: plexio.c child.h plexio.h
io.o: io.c io.h plexio.h
list_test.o: list_test.c list.h
