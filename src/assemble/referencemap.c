#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include "referencemap.h"
#include "../utils/error.h"

// Based on djb2 by Dan Bernstein
unsigned long rmap_hash(const label_t label) {
  unsigned long rmap_hash = 5381;
  for (size_t i = 0; i < strlen(label); i++) {
    rmap_hash = ((rmap_hash << 5) + rmap_hash) + label[i];
  }
  return rmap_hash;
}

/**
 * Get an entry in the map from a particular bucket
 *
 * @param label: string representation of the label
 * @param bucket: pointer to bucket structure containing symbols in bucket
 * @return: corresponding entry for that label in that bucket
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
 * Allocate memory for reference map and set capacity
 *
 * @param capacity: max capacity of the map
 * @return: pointer to the new rmap
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
 * Free up the memory allocated for the map
 *
 * @param map: pointer to the map
 * @return: error code (see error.h)
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
 * Get the list of reference addresses and store it in the given pointer
 *
 * @param map: pointer to the the symbol map object
 * @param label: label object to compare against
 * @param out: preallocated space for the addresses to be placed
 * @param out_size: number of references to copy.
 * @return: error code (see error.h)
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
 * @param address: block of addresses
 * @param count: size of teh address block
 * @param key: address to look for in the block the addresses
 * @return: 1 iff address exists in map
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
 * Check if an address exists in the given map or not
 *
 * @param map: pointer to the the symbol map object
 * @param label: label object to compare against
 * @return: 1 iff label exists in map
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
 * Put an address into the map under the given label
 *
 * @param map: pointer to the the symbol map object
 * @param label: label under which to place the address
 * @param address: address to enter into the map
 * @return: error code (see error.h)
 */
int rmap_put(const reference_map_t *map,
             const label_t label,
             const address_t new_address) {
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
    if (rmap_address_exists(address, entry->references.count, new_address)
        == 1) {
      return -1;
    }

    // Otherwise extend the size of this entry (realloc) to map label to the additional address required
    size_t num_addrs = entry->references.count;
    address = realloc(address, (num_addrs + 1) * sizeof(address_t));
    if (address == NULL) {
      return EC_NULL_POINTER;
    }

    address[num_addrs] = new_address;
    entry->references.count++;
    return EC_OK;

    // There is no entry for the label in the bucket it belongs
  } else {
    int label_len = strlen(label) + 1;
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
      bucket->entries =
          realloc(bucket->entries, (bucket->count + 1) * sizeof(entry_t));
      if (bucket->entries == NULL) {
        free(new_label);
        return EC_NULL_POINTER;
      }
    }
    bucket->count += 1;
    entry = &(bucket->entries[bucket->count - 1]);

    // Copy the label to add into the new entry
    entry->label = new_label;
    memcpy(entry->label, label, label_len);

    // Allocate a new address pointer
    entry->references.address = malloc(sizeof(address_t));

    // If malloc failed
    if (entry->references.address == NULL) {
      free(new_label);
      free(bucket->entries);
      return EC_NULL_POINTER;
    }

    //Place the new_address in
    entry->references.address[0] = new_address;
    entry->references.count = 1;

    return EC_OK;
  }
}


/**
 * Enumerate through each label-address combination and apply the map function
 * to the entry with the object for return/side-effects.
 *
 * @param map: pointer to the the symbol map object
 * @param func: void function that takes a label, address and object params
 * @param obj: object to pass around to each func. Can be null.
 * @return: error code (see error.h)
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
 * Apply the smap_count_func to each entry to get a total count of entries
 *
 * @param map: pointer to the the symbol map object
 * @return: integer for the total count of the map objects
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


void rmap_print_entry(const label_t label, const address_t val,
                      const void *obj) {
  printf("(%s, 0x%08x) \n", label, val);
}

void rmap_print(reference_map_t *map) {
  rmap_enum(map, &rmap_print_entry, NULL);
}
