#ifndef LINES_H
#define LINES_H

#include <stddef.h>

struct lines {
  int capacity;
  int tail;
  int elements;
  char **lines;
};

struct lines * new_lines(size_t n);
void free_lines(struct lines * res);
void lines_insert(struct lines * l, char * line);

#define lines_for_each(i, l) \
  for (i = (l->elements == 0) \
       ? -1 \
       : (l->elements <= l->tail) ? l->tail - l->elements : l->capacity + l->tail - l->elements; \
       0 <= i && i < l->capacity; \
       (i == l->tail-1 || i == l->tail + l->capacity - 1) \
       ? (i = -1) \
       : (i == l->capacity-1) \
	 ? (i = 0) \
	 : (++i))

#endif
