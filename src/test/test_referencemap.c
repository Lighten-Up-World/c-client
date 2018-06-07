#include "unity/unity.h"
#include "../assemble/referencemap.h"

void print_entry(const label_t label, const address_t val, const void *obj) {
  printf("(%s, %d) \n", label, val);
}

void test_referencemap(void) {
  reference_map_t *rm;
  address_t *buf;
  int result;

  rm = rmap_new(10);
  if (rm == NULL) {
      /* Handle allocation failure... */
  }
  char *name = "application name";
  char *ver = "application version";
  /* Insert a couple of string associations */
  rmap_put(rm, name, 1);

  printf("Put ver 2 %u\n", rmap_put(rm, ver, 2));
  printf("Put ver 3 %u\n", rmap_put(rm, ver, 3));
  printf("Put ver 4 %u\n", rmap_put(rm, ver, 4));
  int bucket = rmap_hash(ver) % 10;
  printf("Label: %s %u\n", ver, bucket);

  printf("Size of Bucket %u: %lu\n", bucket, rm->buckets[bucket].count);
  printf("Label 0 of Bucket %u: %s\n", bucket, rm->buckets[bucket].entries[0].label);
  printf("Count of Label 0 of Bucket %u: %lu\n", bucket,
  rm->buckets[bucket].entries[0].references.count);
  printf("Reference 0 of Label 0 of Bucket %u: %04x\n", bucket,
  rm->buckets[bucket].entries[0].references.address[0]);


  rmap_enum(rm, &print_entry, NULL);

  /* Retrieve a address */

  size_t size = rmap_get_references(rm, ver, NULL, 0);
  buf = malloc(size * sizeof(address_t));
  printf("Size of %s: %ld\n", ver, size);
  int res = result = rmap_get_references(rm, ver, buf, size);
  printf("Res %u", res);
  if (result == 0) {
      printf("KEY: %s Not found\n", ver);
  }
  printf("KEY: %s\n", ver);
  for (size_t i = 0; i < size; i++) {
    printf("\t Value: %d\n", buf[i]);
  }

  /* When done, destroy the map object */
  rmap_delete(rm);
}
