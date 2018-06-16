/*
 * Supports an arbitrary linked list data structure, used for storing binary
 * words that should be written at the end of the binary file, and for storing
 * tokens in a program line.
 */


#include "list.h"

/**
 * Create a new linked list element
 *
 * @param value: value for the element
 * @return: pointer to the element
 */
list_elem_t *list_elem_new(any_t value) {
  list_elem_t *self;
  if (!(self = malloc(sizeof(list_elem_t)))) {
    return NULL;
  }
  self->prev = NULL;
  self->next = NULL;
  self->value = value;
  return self;
}

/**
 * Create a new linked list
 *
 * @param v_free: function to free the list
 * @return: pointer to a new heap-allocated list
 */
list_t *list_new(free_func_t v_free) {
  list_t *self;
  if (!(self = malloc(sizeof(list_t)))) {
    return NULL;
  }
  self->head = NULL;
  self->tail = NULL;
  self->v_free = v_free;
  self->len = 0;
  return self;
}

/**
 * Free the individual elements of the list as well as the list itself
 *
 * @param self: pointer to the list to delete
 */
void list_delete(list_t *self) {
  uint32_t len = self->len;
  list_elem_t *next;
  list_elem_t *curr = self->head;

  while (len--) {
    next = curr->next;
    if (self->v_free) {
      self->v_free(curr->value);
    }
    free(curr);
    curr = next;
  }
  free(self);
}

//// List utility functions ////

/**
 * Add an element to the end of a list
 *
 * @param self: list to add element to
 * @param value: value to add to list
 * @return: error code based on success of the addition
 */
int list_add(list_t *self, any_t value) {
  if (!value) {
    return EC_LIST_NULL;
  }
  if (!self->len) {
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

/**
 * Auxiliary function used in list_get to get a list element
 *
 * @param self: list to get element from
 * @param idx: index to get element at
 * @return: list elem searched for or NULL if element not in list
 */
list_elem_t *list_get_elem(list_t *self, int idx) {
  assert(self != NULL);
  assert(idx >= 0);

  if (idx < self->len) {
    list_elem_t *curr = self->head;
    while (idx--) {
      curr = curr->next;
    }
    return curr;
  }
  return NULL;
}

/**
 * Get an item stored in a list
 *
 * @param self: list to get element from
 * @param idx: index in list of element to get
 * @return: value of arbitrary type stored in the list
 */
any_t list_get(list_t *self, int idx) {
  assert(self != NULL);
  assert(idx >= 0);

  list_elem_t *curr;
  if (!(curr = list_get_elem(self, idx))) {
    return NULL;
  }
  return curr->value;
}

/**
 * Remove the element from list at a given index
 *
 * @param self: list to remove element from
 * @param idx: index of element to remove
 * @return; error code returned from list_remove_elem
 */
int list_remove(list_t *self, int idx) {
  return list_remove_elem(self, list_get_elem(self, idx));
}

/**
 * Auxiliary function used in list_remove to remove a list_element
 *
 * @param self: list to remove elem from
 * @param elem: list_elem to remove from list
 * @return: error code based on success of function
 */
int list_remove_elem(list_t *self, list_elem_t *elem) {
  elem->prev ? (elem->prev->next = elem->next)
             : (self->head = elem->next);
  elem->next ? (elem->next->prev = elem->prev)
             : (self->tail = elem->prev);

  if (self->v_free) {
    self->v_free(elem->value);
  }

  free(elem);
  --self->len;
  return EC_LIST_OK;
}

/**
 * Enumeraate through the elements of a list
 *
 * @param self: list to enumerate through
 * @param call: call function to enumerate the list with
 * @param obj: object to call the function on
 * @return: integer error code based on success of function
 */
int list_enum(list_t *self, call_func_t call, any_t obj) {
  int idx = self->len;
  list_elem_t *curr = self->head;
  while (idx--) {
    call(curr->value, obj);
    curr = curr->next;
  }
  return EC_LIST_OK;
}
