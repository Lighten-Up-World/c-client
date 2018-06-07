#include "symbolmap.h"
#include "../utils/error.h"

// Based on djb2 by Dan Bernstein
unsigned long hash(const label_t label){
  unsigned long hash = 5381;
  for (size_t i = 0; i < strlen(label); i++) {
    hash = ((hash << 5) + hash) + label[i];
  }
	return hash;
}

/**
* Gets a symbol entry in the map from a particular bucket
*
* @param label : String representation of the label
* @param bucket : Pointer to bucket structure containing symbols in bucket
* @returns The corresponding symbol for that label in that bucket.
*/
static symbol_t *get_symbol(sbucket_t *bucket, const label_t label){
  size_t n = bucket->count;
	if (n == 0) {
		return NULL;
	}
  symbol_t *symbol = bucket->symbols;
  for(size_t i = 0; i < n; i++){
    if(symbol->label != NULL){
      if(strcmp(symbol->label, label) == 0){
        return symbol;
      }
    }
    symbol++;
  }
  return NULL;
}

/**
* Allocates memory for symbol map and sets capacity
*
* @param capacity : Max capacity of the map
* @returns A pointer to the new smap
*/
symbol_map_t *smap_new(size_t capacity){
  symbol_map_t *map;
  map = calloc(1, sizeof(symbol_map_t));
  if(map == NULL){
    return NULL;
  }
  map->count = capacity;
  map->buckets = calloc(map->count, sizeof(sbucket_t));
  if(map->buckets == NULL){
    free(map);
    return NULL;
  }
  return map;
}

/**
* Frees up the memory allocated for the map
*
* @param map : Pointer to the map
* @returns An error code (see error.h)
*/
int smap_delete(symbol_map_t *map){
  if(map == NULL){
    return EC_NULL_POINTER;
  }
  sbucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    symbol_t *symbol = bucket->symbols;
    for(size_t j = 0; j < bucket->count; j++){
      free(symbol->label);
      symbol++;
    }
    free(bucket->symbols);
    bucket++;
  }
  free(map->buckets);
  free(map);
  return EC_OK;
}

/**
* Gets an address and stores it in the given pointer
*
* @param map : Pointer to the the symbol map object
* @param label : The label object to compare against
* @param out : The preallocated space for the address to be placed
* @returns An error code (see error.h)
*/
int smap_get_address(const symbol_map_t *map, const label_t label, address_t *out){
  if(map == NULL){
    return EC_INVALID_PARAM;
  }
  if(label == NULL){
    return EC_INVALID_PARAM;
  }
  if(out == NULL){
    return EC_INVALID_PARAM;
  }
  size_t ind = hash(label) % map->count;
  sbucket_t *bucket = &(map->buckets[ind]);
  symbol_t *symbol = get_symbol(bucket, label);
  if(symbol == NULL){
    return EC_NULL_POINTER;
  }
  *out = symbol->address;
  return EC_OK;
}

/**
* Checks if a label exists in the given map or not
*
* @param map : Pointer to the the symbol map object
* @param label : The label object to compare against
* @returns 1 iff label exists in map
*/
int smap_exists(const symbol_map_t *map, const label_t label){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){
    return 0;
  }
  size_t ind = hash(label) % map->count;
  sbucket_t *bucket = &(map->buckets[ind]);
  symbol_t *symbol = get_symbol(bucket, label);
  if(symbol == NULL){
    return 0;
  }
  return 1;
}

/**
* Puts a label-address pair as an entry into the map
*
* @param map : Pointer to the the symbol map object
* @param label : The label to place in the map
* @param address : The address to enter into the map
* @returns An error code (see error.h)
*/
int smap_put(const symbol_map_t *map, const label_t label, const address_t address){
  if(map == NULL){
    return EC_INVALID_PARAM;
  }
  if(label == NULL){
    return EC_INVALID_PARAM;
  }
  size_t label_len = strlen(label);
  size_t address_len = sizeof(address_t);
  size_t ind = hash(label) % map->count;
  sbucket_t *bucket = &(map->buckets[ind]);

  symbol_t *symbol;
  if((symbol = get_symbol(bucket, label)) != NULL){
    symbol->address = address;
    return EC_NULL_POINTER;
  }
  label_t new_label = malloc(label_len);
  if(new_label == NULL){
    return EC_NULL_POINTER;
  }
  address_t new_address = address;
  if(bucket->count == 0){
    bucket->symbols = malloc(sizeof(symbol_t));
    if(bucket->symbols == NULL){
      free(new_label);
      return EC_NULL_POINTER;
    }
    bucket->count = 1;
  }
  else {
    bucket->count += 1;
    symbol_t *tmp_symbols = realloc(bucket->symbols, bucket->count * sizeof(symbol_t));
    if(tmp_symbols == NULL){
      free(new_label);
      bucket->count -= 1;
      return EC_NULL_POINTER;
    }
    bucket->symbols = tmp_symbols;
  }
  symbol = &(bucket->symbols[bucket->count - 1]);
  symbol->label = new_label;
  memcpy(symbol->label, label, label_len);
  symbol->address = new_address;
  return EC_OK;
}

/**
* Enumerates through each label-address pair and applies the map function
* to the entry with the object for return/side-effects.
*
* @param map : Pointer to the the symbol map object
* @param func : A void function that take a label, address and object params
* @param obj : The object to pass around to each func. Can be null.
* @returns An error code (see error.h)
*/
int smap_enum(symbol_map_t *map, map_func_t func, const void *obj){
  if(map == NULL){
    return EC_INVALID_PARAM;
  }
  if(func == NULL){
    return EC_INVALID_PARAM;
  }
  sbucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    symbol_t *symbol = bucket->symbols;
    for(size_t j = 0; j < bucket->count; j++){
      func(symbol->label, symbol->address, obj);
      symbol++;
    }
    bucket++;
  }
  return EC_OK;
}

/**
* Simple map function that takes a pointer to a size_t object and increments it
*/
void smap_count_func(const label_t label, const address_t val, const void *obj){
  size_t *cnt = (size_t *) obj;
  *cnt += 1;
}

/**
* Applies the smap_count_func to each entry to get a total count of entries
*
* @param map : Pointer to the the symbol map object
* @returns An integer for the total count of the map objects
*/
int smap_get_count(symbol_map_t *map){
  size_t count = 0;
  smap_enum(map, &smap_count_func, &count);
  return count;
}