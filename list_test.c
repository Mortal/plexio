#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  struct list * l = new_list();
  for (int i = 0; i < (1 << 6); ++i) {
    list_insert(l, i);
  }
  for_each_list(l, l_, i, el) {
    printf("%d\n", el);
  }
  return 0;
}
