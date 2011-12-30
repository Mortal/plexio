#include <stdlib.h>
#include <string.h>
#include "list.h"

struct list * new_list() {
  struct list * res = (struct list *) malloc(sizeof(struct list));
  res->next = res;
  res->el_count = 0;
  memset(res->elements, -1, sizeof(int) * LIST_ELEMENTS);
  return res;
}

void free_list(struct list * l) {
  free(l);
}

void list_insert(struct list * l, int el) {
  for_each_list(l, l_, i, cur) {
    if (cur == -1) {
      l_->elements[i] = el;
      return;
    }
  }
  if (i < LIST_ELEMENTS) {
    l_->elements[i] = el;
    ++l_->el_count;
    return;
  }
  struct list * tail = new_list();
  l_->next = tail;
  list_insert(tail, el);
}
