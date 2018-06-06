#ifndef REFERENCE_MAP_H
#define REFERENCE_MAP_H

#include <stdlib.h>
#include <string.h>
#include "../utils/arm.h"

#define MAX_R_MAP_CAPACITY 30

typedef char * label_t;
typedef void (*map_func_t) (const label_t label, const address_t val, const void *obj);

typedef struct {
  address_t *address;
  size_t count;
} references_t;

typedef struct {
  label_t label;
  references_t references;
} entry_t;

typedef struct {
  size_t count;
  entry_t *entries;
} rbucket_t;

typedef struct {
  size_t count;
  rbucket_t *buckets;
} reference_map_t;

unsigned long hash(const label_t label);

reference_map_t *rmap_new(size_t capacity);

int rmap_delete(reference_map_t *map);

int rmap_get_references(const reference_map_t *map, const label_t label,
                      address_t *out, size_t out_size);
int rmap_exists(const reference_map_t *map, const label_t label);
int rmap_put(const reference_map_t *map, const label_t label, const address_t address);

int rmap_get_count(reference_map_t *map);

int rmap_enum(reference_map_t *map, map_func_t func, const void *obj);

#endif
