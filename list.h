#ifndef LIST_H
#define LIST_H

#define LIST_ELEMENTS (16)

struct list {
  struct list * next;
  int el_count;
  int elements[LIST_ELEMENTS];
};

struct list * new_list();
void free_list(struct list * l);
void list_insert(struct list * l, int el);

#define for_each_list(list_src, list_tmp, i, element) \
  int i, element; \
  struct list * list_tmp = list_src; \
  element = list_src->elements[0]; \
  for (i = 0; i < list_tmp->el_count; \
       (i < list_tmp->el_count-1) ? element = list_tmp->elements[++i] \
       : (list_tmp != list_tmp->next) \
       ? (i = 0, list_tmp = list_tmp->next, element = list_tmp->elements[0]) \
       : (++i))

#endif
