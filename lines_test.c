#include <stdio.h>
#include <string.h>
#include "lines.h"

int main() {
  struct lines * l = new_lines(3);
  int i;

  lines_for_each(i, l) printf("%s\n", l->lines[i]);
  printf("--\n");

  lines_insert(l, strdup("line 1"));
  lines_insert(l, strdup("line 2"));
  lines_for_each(i, l) printf("%s\n", l->lines[i]);
  printf("--\n");

  lines_insert(l, strdup("line 3"));
  lines_insert(l, strdup("line 4"));
  lines_for_each(i, l) printf("%s\n", l->lines[i]);
  printf("--\n");

  return 0;
}
