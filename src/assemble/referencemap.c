#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include "referencemap.h"
#include "../utils/error.h"

// Based on djb2 by Dan Bernstein
unsigned long rmap_hash(const label_t label){
  unsigned long rmap_hash = 5381;
  for (size_t i = 0; i < strlen(label); i++) {
    rmap_hash = ((rmap_hash << 5) + rmap_hash) + label[i];
  }
	return rmap_hash;
}

/**
* Gets an entry in the map from a particular bucket
*
* @param label : String representation of the label
* @param bucket : Pointer to bucket structure containing symbols in bucket
* @returns The corresponding entry for that label in that bucket.
*/
static entry_t *get_entry(rbucket_t *bucket, const label_t label) {
  size_t n = bucket->count;
  if (n == 0) {
    return NULL;
  }
  entry_t *entry = bucket->entries;
  for (size_t i = 0; i < n; i++) {
    if (entry->label != NULL) {
      if (strcmp(entry->label, label) == 0) {
        return entry;
      }
    }
    entry++;
  }
  return NULL;
}

/**
* Allocates memory for reference map and sets capacity
*
* @param capacity : Max capacity of the map
* @returns A pointer to the new rmap
*/
reference_map_t *rmap_new(size_t capacity) {
  reference_map_t *map;
  map = calloc(1, sizeof(reference_map_t));
  if (map == NULL) {
    return NULL;
  }
  map->count = capacity;
  map->buckets = calloc(map->count, sizeof(rbucket_t));
  if (map->buckets == NULL) {
    free(map);
    //ERROR
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
int rmap_delete(reference_map_t *map) {
  if (map == NULL) {
    return EC_NULL_POINTER;
  }
  rbucket_t *bucket = map->buckets;
  for (size_t i = 0; i < map->count; i++) {
    entry_t *entry = bucket->entries;
    for (size_t j = 0; j < bucket->count; j++) {
      free(entry->label);
      free(entry->references.address);
      entry++;
    }
    free(bucket->entries);
    bucket++;
  }
  free(map->buckets);
  free(map);
  return EC_OK;
}

/**
* Gets the list of reference addresses and stores it in the given pointer
*
* @param map : Pointer to the the symbol map object
* @param label : The label object to compare against
* @param out : The preallocated space for the addresses to be placed
* @param out_size : The number of references to copy.
* @returns An error code (see error.h)
*          or the number of references if out_size < reference count
*/
int rmap_get_references(const reference_map_t *map, const label_t label,
                        address_t *out, size_t out_size) {
  if (map == NULL) {
    return EC_INVALID_PARAM;
  }
  if (label == NULL) {
    return EC_INVALID_PARAM;
  }
  size_t ind = rmap_hash(label) % map->count;
  rbucket_t *bucket = &(map->buckets[ind]);
  entry_t *entry = get_entry(bucket, label);
  if (entry == NULL) {
    return EC_NULL_POINTER;
  }
  if (out_size < entry->references.count) {
    return entry->references.count;
  }
  if (out == NULL) {
    return EC_INVALID_PARAM;
  }
  memcpy(out, entry->references.address, out_size * sizeof(address_t));
  return EC_OK;
}

/**
* Checks if an address exists in the given map or not
*
* @param address : Block of addresses
* @param count : Size of teh address block
* @param key : the address to look for in the block the addresses
* @returns 1 iff address exists in map
*/
int rmap_address_exists(address_t *address, size_t count, address_t key) {
  if (address == NULL) {
    return 0;
  }
  for (size_t i = 0; i < count; i++) {
    if (address[i] == key) {
      return 1;
    }
  }
  return 0;
}

/**
* Checks if an address exists in the given map or not
*
* @param map : Pointer to the the symbol map object
* @param label : The label object to compare against
* @returns 1 iff label exists in map
*/
int rmap_exists(const reference_map_t *map, const label_t label) {
  if (map == NULL) {
    return 0;
  }
  if (label == NULL) {
    return 0;
  }
  size_t ind = rmap_hash(label) % map->count;
  rbucket_t *bucket = &(map->buckets[ind]);
  entry_t *entry = get_entry(bucket, label);
  if (entry == NULL) {
    return 0;
  }
  return 1;
}

/**
* Puts a address into the map under the given label
*
* @param map : Pointer to the the symbol map object
* @param label : The label under which to place the address
* @param address : The address to enter into the map
* @returns An error code (see error.h)
*/
int rmap_put(const reference_map_t *map, const label_t label, const address_t new_address) {
  if (map == NULL || label == NULL) {
    return EC_INVALID_PARAM;
  }

  // Calculate which bucket the entry should be placed in
  size_t index = rmap_hash(label) % map->count;
  rbucket_t *bucket = &(map->buckets[index]);

  // If there is already an entry for the label in the bucket it belongs
  entry_t *entry;
  if ((entry = get_entry(bucket, label)) != NULL) {
    address_t *address = entry->references.address;

    // If this label/address pair is already in the map return error code
    if (rmap_address_exists(address, entry->references.count, new_address) == 1) {
      return -1;

    // Otherwise extend the size of this entry (realloc) to map label to the additional address required
    } else {
      size_t num_addrs = entry->references.count;
      address = realloc(address, (num_addrs + 1) * sizeof(address_t));
      if (address == NULL) {
        return EC_NULL_POINTER;
      }
      address[num_addrs] = new_address;
      entry->references.count++;
      return EC_OK;
    }

  // There is no entry for the label in the bucket it belongs
  } else {
    size_t label_len = strlen(label);
    label_t new_label = malloc(label_len);
    if (new_label == NULL) {
      return EC_NULL_POINTER;
    }

    // If there are no entries in the bucket, allocate an entry
    if (bucket->count == 0) {
      bucket->entries = malloc(sizeof(entry_t));
      if (bucket->entries == NULL) {
        free(new_label);
        return EC_NULL_POINTER;
      }

    // There are already entries in the bucket, increase size to store 1 more
    } else {
      bucket->entries = realloc(bucket->entries, (bucket->count + 1) * sizeof(entry_t));
      if (bucket->entries == NULL) {
        free(new_label);
        return EC_NULL_POINTER;
      }
    }
    bucket->count += 1;
    entry = &(bucket->entries[bucket->count - 1]);

    size_t num_addrs = entry->references.count;

    // Copy the label to add into the new entry
    entry->label = new_label;
    memcpy(entry->label, label, label_len);

    //Case: if the references array with size is empty
    if (num_addrs == 0) {
      // Allocate a new address pointer
      entry->references.address = malloc(sizeof(address_t));

      printf("malloc here\n");
      // If malloc filed
      if (entry->references.address == NULL) {
        free(new_label);
        free(bucket->entries);
        return EC_NULL_POINTER;
      }
      //Set refrences.count to 1
      entry->references.count = 1;
    }
    // References array with size is not empty
    else {

      printf("realloc here\n");
      // Increment the count
      entry->references.count += 1;
      //Reallocate the address to be one bigger
      entry->references.address = realloc(entry->references.address, entry->references.count * sizeof(address_t));

      // Check to see if the realloc failed
      if (entry->references.address == NULL) {
        free(new_label);
        free(bucket->entries);
        bucket->count -= 1;
        return EC_NULL_POINTER;
      }
    }
    //Place the new_address in
    entry->references.address[num_addrs] = new_address;

    return EC_OK;
  }
}


/**
* Enumerates through each label-address combination and applies the map function
* to the entry with the object for return/side-effects.
*
* @param map : Pointer to the the symbol map object
* @param func : A void function that take a label, address and object params
* @param obj : The object to pass around to each func. Can be null.
* @returns An error code (see error.h)
*/
int rmap_enum(reference_map_t *map, map_func_t func, const void *obj) {
  if (map == NULL) {
    return EC_INVALID_PARAM;
  }
  if (func == NULL) {
    return EC_INVALID_PARAM;
  }
  rbucket_t *bucket = map->buckets;
  for (size_t i = 0; i < map->count; i++) {
    entry_t *entry = bucket->entries;
    for (size_t j = 0; j < bucket->count; j++) {
      address_t *address = entry->references.address;
      for (size_t k = 0; k < entry->references.count; k++) {
        func(entry->label, *address, obj);
        address++;
      }
      entry++;
    }
    bucket++;
  }
  return EC_OK;
}

/**
* Applies the smap_count_func to each entry to get a total count of entries
*
* @param map : Pointer to the the symbol map object
* @returns An integer for the total count of the map objects
*/
int rmap_get_count(reference_map_t *map) {
  size_t count = 0;
  if (map == NULL) {
    return EC_INVALID_PARAM;
  }
  rbucket_t *bucket = map->buckets;
  for (size_t i = 0; i < map->count; i++) {
    entry_t *entry = bucket->entries;
    for (size_t j = 0; j < bucket->count; j++) {
      count++;
      entry++;
    }
    bucket++;
  }
  return count;
}


void rmap_print_entry(const label_t label, const address_t val, const void *obj) {
  printf("(%s, 0x%08x) \n", label, val);
}

void rmap_print(reference_map_t *map){
  rmap_enum(map, &rmap_print_entry, NULL);
}
