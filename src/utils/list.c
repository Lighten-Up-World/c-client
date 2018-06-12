#include "list.h"

list_elem_t *list_elem_new(any_t value){
  list_elem_t *self;
  if(!(self = malloc(sizeof(list_elem_t)))){
    return NULL;
  }
  self->prev = NULL;
  self->next = NULL;
  self->value = value;
  return self;
}

list_t *list_new(free_func_t v_free){
  list_t *self;
  if(!(self = malloc(sizeof(list_t)))){
    return NULL;
  }
  self->head = NULL;
  self->tail = NULL;
  self->v_free = v_free;
  self->len = 0;
  return self;
}

void list_destroy(list_t *self){
  uint32_t len = self->len;
  list_elem_t *next;
  list_elem_t *curr = self->head;

  while(len--){
    next = curr->next;
    if(self->v_free){
      self->v_free(curr->value);
    }
    free(curr);
    curr = next;
  }
  free(self);
}

// List Functions
int list_add(list_t *self, any_t value){
  if(!value){
    return EC_LIST_NULL;
  }
  if(!self->len){
    self->head = self->tail = list_elem_new(value);
    self->head->prev = self->head->next = NULL;
  } else {
    list_elem_t *elem = list_elem_new(value);
    elem->prev = self->tail;
    elem->next = NULL;
    self->tail->next = elem;
    self->tail = elem;
  }
  ++self->len;
  return EC_LIST_OK;
}
list_elem_t *list_get_elem(list_t *self, int idx){
  assert(self != NULL);
  assert(idx >= 0);

  if(idx < self->len){
    list_elem_t *curr = self->head;
    while(idx--){
      curr = curr->next;
    }
    return curr;
  }
  return NULL;
}

any_t list_get(list_t *self, int idx){
  assert(self != NULL);
  assert(idx >= 0);
  list_elem_t *curr;
  if(!(curr = list_get_elem(self, idx))){
    return NULL;
  }
  return curr->value;
}

int list_remove(list_t *self, int idx){
  return list_remove_elem(self, list_get_elem(self, idx));
}

int list_remove_elem(list_t *self, list_elem_t *elem){
  elem->prev ? (elem->prev->next = elem->next)
             : (self->head = elem->next);
  elem->next ? (elem->next->prev = elem->prev)
             : (self->tail = elem->prev);

  if(self->v_free){
    self->v_free(elem->value);
  }

  free(elem);
  --self->len;
  return EC_LIST_OK;
}

int list_enum(list_t *self, call_func_t call, any_t obj){
  int idx = self->len;
  list_elem_t *curr = self->head;
  while(idx--){
    call(curr->value, obj);
    curr = curr->next;
  }
  return EC_LIST_OK;
}
