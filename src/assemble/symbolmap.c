#include "map.h"
#include "symbolmap.h"


static symbol_t *get_symbol(bucket_t *bucket, const label_t label){
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


symbol_map_t *smap_new(size_t capacity){
  symbol_map_t *map;
  map = calloc(1, sizeof(symbol_map_t));
  if(map == NULL){
    //ERROR
    return NULL;
  }
  map->count = capacity;
  map->buckets = calloc(map->count, sizeof(bucket_t));
  if(map->buckets == NULL){
    free(map);
    //ERROR
    return NULL;
  }
  return map;
}


int smap_delete(symbol_map_t *map){
  if(map == NULL){
    return 0;
  }
  bucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    symbol_t *symbol = bucket->symbols;
    for(size_t j = 0; j < bucket->count; j++){
      free(symbol->label);
      // free(symbol->address);
      symbol++;
    }
    free(bucket->symbols);
    bucket++;
  }
  free(map->buckets);
  free(map);
  return 1;
}


int smap_get_address(const symbol_map_t *map, const label_t label, address_t *out, size_t out_size){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){
    return 0;
  }
  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);
  symbol_t *symbol = get_symbol(bucket, label);
  if(symbol == NULL){
    return 0;
  }
  // if(out_size == 0){
  //   return map->val_len(symbol->address); //Different list function
  // }
  if(out == NULL){
    return 0;
  }
  *out = symbol->address;
  //memcpy(out, symbol->address, out_size);
  return 1;
}


int smap_exists(const symbol_map_t *map, const label_t label){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){
    return 0;
  }
  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);
  symbol_t *symbol = get_symbol(bucket, label);
  if(symbol == NULL){
    return 0;
  }
  return 1;
}


int smap_put(const symbol_map_t *map, const label_t label, const address_t address){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){// || address == NULL){
    return 0;
  }
  size_t label_len = strlen(label);
  size_t address_len = sizeof(address_t);
  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);

  symbol_t *symbol;
  if((symbol = get_symbol(bucket, label)) != NULL){
    //address_t tmp_address = realloc(symbol->address, address_len * sizeof(char));
    // if(tmp_address == NULL){
    //   return 0;
    // }
    //symbol->address = tmp_address;
    symbol->address = address;
    return 1;
  }
  label_t new_label = malloc(label_len);
  if(new_label == NULL){
    return 0;
  }
  address_t new_address = address;
  //address_t *new_address = malloc(address_len * sizeof(char));
  // if(new_address == NULL){
  //   free(new_label);
  //   return 0;
  // }
  if(bucket->count == 0){
    bucket->symbols = malloc(sizeof(symbol_t));
    if(bucket->symbols == NULL){
      free(new_label);
      // free(new_address);
      return 0;
    }
    bucket->count = 1;
  }
  else {
    bucket->count += 1;
    symbol_t *tmp_symbols = realloc(bucket->symbols, bucket->count * sizeof(symbol_t));
    if(tmp_symbols == NULL){
      free(new_label);
      // free(new_address);
      bucket->count -= 1;
      return 0;
    }
    bucket->symbols = tmp_symbols;
  }
  symbol = &(bucket->symbols[bucket->count - 1]);
  symbol->label = new_label;
  memcpy(symbol->label, label, label_len);
  symbol->address = new_address;
  //memcpy(symbol->address, address, val_len);
  return 1;
}


int smap_enum(symbol_map_t *map, map_func_t func, const void *obj){
  if(map == NULL){
    return 0;
  }
  if(func == NULL){
    return 0;
  }
  bucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    symbol_t *symbol = bucket->symbols;
    for(size_t j = 0; j < bucket->count; j++){
      func(symbol->label, symbol->address, obj);
      symbol++;
    }
    bucket++;
  }
  return 1;
}


void smap_count_func(const label_t label, const address_t val, const void *obj){
  size_t *cnt = (size_t *) obj;
  *cnt += 1;
}


int smap_get_count(symbol_map_t *map){
  size_t count = 0;
  smap_enum(map, &smap_count_func, &count);
  return count;
}


void print_symbol(const label_t label, const address_t val, const void *obj){
  printf("(%s, %d) \n", label, val);
}

// MOVE TO TEST
// int main(int argc, char const *argv[]) {
//   symbol_map_t *sm;
//   address_t buf;
//   int result;
//
//   sm = smap_new(10);
//   if (sm == NULL) {
//       /* Handle allocation failure... */
//   }
//   /* Insert a couple of string associations */
//   smap_put(sm, "application name", 1);
//   smap_put(sm, "application version", 2);
//
//   smap_enum(sm, &print_symbol, NULL);
//   /* Retrieve a address */
//   result = smap_get_address(sm, "application name", &buf, sizeof(buf));
//   if (result == 0) {
//       printf("KEY: %s Not found\n", "application name");
//   }
//   printf("KEY: %s, Value: %d\n", "application name", buf);
//
//   /* When done, destroy the map object */
//   smap_delete(sm);
//   return 0;
// }
