#ifndef LIST_H
#define LIST_H

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define EC_LIST_OK 0
#define EC_LIST_NULL 1

typedef void * any_t;

typedef void (*free_func_t) (any_t value);
typedef void (*call_func_t) (any_t value, any_t obj);

typedef struct list_elem {
  struct list_elem *prev;
  struct list_elem *next;
  any_t value;
} list_elem_t;

typedef struct {
  list_elem_t *head;
  list_elem_t *tail;
  uint32_t len;
  free_func_t v_free;
} list_t;

// Node Functions
list_elem_t *list_elem_new(any_t value);

// List Construction
list_t *list_new(free_func_t);
void list_delete(list_t *self);

// List manipulation
int list_add(list_t *self, any_t value);
int list_add_elem(list_t *self, list_elem_t *elem);
any_t list_get(list_t *self, int idx);
int list_remove(list_t *self, int idx);
int list_remove_elem(list_t *self, list_elem_t *node);
int list_enum(list_t *self, call_func_t call, any_t obj);

#endif
