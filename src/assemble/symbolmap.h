#ifndef SYMBOL_MAP_H
#define SYMBOL_MAP_H

#include <stdlib.h>
#include <string.h>
#include "arm.h"

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

symbol_map_t *smap_new(size_t capacity);
int smap_delete(symbol_map_t *map);

int smap_get_address(const symbol_map_t *map, const label_t label, address_t *out, size_t out_size);
int smap_exists(const symbol_map_t *map, const label_t label);
int smap_put(const symbol_map_t *map, const label_t label, const address_t address);

int smap_get_count(symbol_map_t *map);

int smap_enum(symbol_map_t *map, map_func_t func, const void *obj);

#endif
