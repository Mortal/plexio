CFLAGS=-g -Wall -Wextra
all: plexio

clean:
	rm -f *.o plexio

plexio: plexio.o child.o io.o

plexio.o: plexio.c child.h plexio.h io.h
child.o: plexio.c child.h plexio.h
io.o: io.c io.h plexio.h
