#include <stdio.h>
#include <strings.h>
#include "map.h"
#include "referencemap.h"

static entry_t *get_entry(bucket_t *bucket, const label_t label){
  size_t n = bucket->count;
	if (n == 0) {
		return NULL;
	}
  entry_t *entry = bucket->entries;
  for(size_t i = 0; i < n; i++){
    if(entry->label != NULL){
      if(strcmp(entry->label, label) == 0){
        return entry;
      }
    }
    entry++;
  }
  return NULL;
}

reference_map_t *rmap_new(size_t capacity){
  reference_map_t *map;
  map = calloc(1, sizeof(reference_map_t));
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

int rmap_delete(reference_map_t *map){
  if(map == NULL){
    return 0;
  }
  bucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    entry_t *entry = bucket->entries;
    for(size_t j = 0; j < bucket->count; j++){
      free(entry->label);
      free(entry->references.address);
      entry++;
    }
    free(bucket->entries);
    bucket++;
  }
  free(map->buckets);
  free(map);
  return 1;
}

int rmap_get_references(const reference_map_t *map, const label_t label, address_t *out, size_t out_size){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){
    return 0;
  }
  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);
  entry_t *entry = get_entry(bucket, label);
  if(entry == NULL){
    return 0;
  }
  if(out_size < entry->references.count){
    return entry->references.count;
  }
  if(out == NULL){
    return 0;
  }
  memcpy(out, entry->references.address, out_size * sizeof(address_t));
  return 1;
}

int rmap_address_exists(address_t *address, size_t count, address_t new_address){
  if(address == NULL){// || address == NULL){
    return 0;
  }
  for (size_t i = 0; i < count; i++) {
    if(address[i] == new_address){
      return 1;
    }
  }
  return 0;
}

int rmap_exists(const reference_map_t *map, const label_t label){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){
    return 0;
  }
  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);
  entry_t *entry = get_entry(bucket, label);
  if(entry == NULL){
    return 0;
  }
  return 1;
}
int rmap_put(const reference_map_t *map, const label_t label, const address_t new_address){
  if(map == NULL){
    return 0;
  }
  if(label == NULL){// || address == NULL){
    return 0;
  }
  size_t label_len = strlen(label);

  size_t ind = hash(label) % map->count;
  bucket_t *bucket = &(map->buckets[ind]);

  entry_t *entry;
  if((entry = get_entry(bucket, label)) != NULL){
    address_t *address = entry->references.address;
    if(rmap_address_exists(address, entry->references.count, new_address) == 1){
      return 0;
    }
    else {
      size_t address_len = entry->references.count;
      address_t *tmp_references_address = realloc(address, (address_len + 1) * sizeof(address_t));
      if(tmp_references_address == NULL){
        return 0;
      }
      address = tmp_references_address;
      address[address_len] = new_address;
      entry->references.count ++;
      return 1;
    }
  }

  label_t new_label = malloc(label_len);
  if(new_label == NULL){
    return 0;
  }

  if(bucket->count = 0){
    bucket->entries = malloc(sizeof(entry_t));
    if(bucket->entries == NULL){
      free(new_label);
      return 0;
    }
    bucket->count = 1;
  }
  else {
    bucket->count += 1;
    entry_t *tmp_entries = realloc(bucket->entries, bucket->count * sizeof(entry_t));
    if(tmp_entries == NULL){
      free(new_label);
      bucket->count -= 1;
      return 0;
    }
    bucket->entries = tmp_entries;

  }

  entry = &(bucket->entries[bucket->count - 1]);

  size_t address_len = entry->references.count;

  entry->label = new_label;
  memcpy(entry->label, label, label_len);



  if(address_len == 0){
    entry->references.address = malloc(sizeof(address_t));
    if(entry->references.address == NULL){
      free(new_label);
      free(bucket->entries);
      return 0;
    }
    entry->references.count = 1;
  }
  else {
    entry->references.count += 1;
    address_t *tmp_references_address = realloc(entry->references.address, entry->references.count * sizeof(address_t));
    if(tmp_references_address == NULL){
      free(new_label);
      free(bucket->entries);
      bucket->count -= 1;
      return 0;
    }
    entry->references.address = tmp_references_address;
  }
  printf("address_len %lu\n", address_len);
  printf("Current %lu\n", entry->references.count);
  entry->references.address[address_len] = new_address;
  return 1;
}

int rmap_enum(reference_map_t *map, map_func_t func, const void *obj){
  if(map == NULL){
    return 0;
  }
  if(func == NULL){
    return 0;
  }
  bucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    entry_t *entry = bucket->entries;
    for(size_t j = 0; j < bucket->count; j++){
      address_t *address = entry->references.address;
      for (size_t k = 0; k < entry->references.count; k++) {
        func(entry->label, *address, obj);
        address++;
      }
      entry++;
    }
    bucket++;
  }
  return 1;
}

int rmap_get_count(reference_map_t *map){
  size_t count = 0;
  if(map == NULL){
    return 0;
  }
  bucket_t *bucket = map->buckets;
  for(size_t i = 0; i < map->count; i++){
    entry_t *entry = bucket->entries;
    for(size_t j = 0; j < bucket->count; j++){
      count++;
      entry++;
    }
    bucket++;
  }
  return count;
}

void print_entry(const label_t label, const address_t val, const void *obj){
  printf("(%s, %d) \n", label, val);
}
// MOVE TO TEST
// int main(int argc, char const *argv[]) {
//   reference_map_t *rm;
//   address_t *buf;
//   int result;
//
//   rm = rmap_new(10);
//   if (rm == NULL) {
//       /* Handle allocation failure... */
//   }
//   char *name = "application name";
//   char *ver = "application version";
//   /* Insert a couple of string associations */
//   rmap_put(rm, name, 1);
//
//   printf("Put ver 2 %u\n", rmap_put(rm, ver, 2));
//   printf("Put ver 3 %u\n", rmap_put(rm, ver, 3));
//   printf("Put ver 4 %u\n", rmap_put(rm, ver, 4));
//   int bucket = hash(ver) % 10;
//   printf("Label: %s %u\n", ver, bucket);
//
//   printf("Size of Bucket %u: %lu\n", bucket, rm->buckets[bucket].count);
//   printf("Label 0 of Bucket %u: %s\n", bucket, rm->buckets[bucket].entries[0].label);
//   printf("Count of Label 0 of Bucket %u: %lu\n", bucket,
//   rm->buckets[bucket].entries[0].references.count);
//   printf("Reference 0 of Label 0 of Bucket %u: %04x\n", bucket,
//   rm->buckets[bucket].entries[0].references.address[0]);
//
//
//   rmap_enum(rm, &print_entry, NULL);
//
//   /* Retrieve a address */
//
//   size_t size = rmap_get_references(rm, ver, NULL, 0);
//   buf = malloc(size * sizeof(address_t));
//   printf("Size of %s: %ld\n", ver, size);
//   int res = result = rmap_get_references(rm, ver, buf, size);
//   printf("Res %u", res);
//   if (result == 0) {
//       printf("KEY: %s Not found\n", ver);
//   }
//   printf("KEY: %s\n", ver);
//   for (size_t i = 0; i < size; i++) {
//     printf("\t Value: %d\n", buf[i]);
//   }
//
//   /* When done, destroy the map object */
//   rmap_delete(rm);
//   return 0;
// }
