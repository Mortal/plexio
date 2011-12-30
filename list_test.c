#include <stdio.h>
#include "list.h"

int main() {
  struct list * l = new_list();
  for (int i = 0; i < (1 << 5); ++i) {
    list_insert(l, (1 << 6) - i);
  }
  for (int i = 0; i < (1 << 5); i += 3) {
    list_find_and_remove(l, (1 << 6) - i);
  }
  for_each_list(l, l_, i, el) {
    if (el == -1) continue;
    printf("%d\n", el);
  }
  return 0;
}
