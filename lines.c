#include "lines.h"

struct lines * new_lines(size_t n) {
  struct lines * res = (struct lines *) malloc(sizeof(struct lines *));
  res->capacity = n;
  res->tail = 0;
  res->lines = (char **) malloc(n*sizeof(char *));
  return res;
}

void free_lines(struct lines * res) {
  free(res->lines);
  free(res);
}

void lines_insert(struct lines * l, char * line) {
  if (l->lines[l->tail]) free(l->lines[l->tail]);
  l->lines[l->tail] = line;
  l->tail++;
  if (l->tail >= l->capacity) l->tail = 0;
  if (l->elements < l->capacity) ++l->elements;
}
