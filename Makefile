CFLAGS=-g -Wall -Wextra
all: plexio

clean:
	rm -f *.o plexio

plexio: plexio.o child.o

plexio.o: plexio.c child.h plexio.h
child.o: plexio.c child.h plexio.h
