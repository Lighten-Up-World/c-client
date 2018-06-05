#ifndef SYMBOL_MAP_H
#define SYMBOL_MAP_H

#include <stdlib.h>
#include <string.h>
#include "arm.h"

#define MAX_S_MAP_CAPACITY 20

typedef char * label_t;
typedef void (*map_func_t) (const label_t label, const address_t val, const void *obj);

typedef struct {
  label_t label;
  address_t address;
} symbol_t;

typedef struct {
  size_t count;
  symbol_t *symbols;
} bucket_t;

typedef struct {
  size_t count;
  bucket_t *buckets;
} symbol_map_t;

unsigned long hash(const label_t label);

symbol_map_t *smap_new(size_t capacity);
int smap_delete(symbol_map_t *map);

int smap_get_address(const symbol_map_t *map, const label_t label, address_t *out);
int smap_exists(const symbol_map_t *map, const label_t label);
int smap_put(const symbol_map_t *map, const label_t label, const address_t address);

int smap_get_count(symbol_map_t *map);

int smap_enum(symbol_map_t *map, map_func_t func, const void *obj);

#endif
