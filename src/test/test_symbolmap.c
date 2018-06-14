#include "../../unity/src/unity.h"
#include "../assemble/symbolmap.h"

/**
* A map function for printing a symbol
*/
void print_symbol(const label_t label, const address_t val, const void *obj){
  printf("(%s, %d) \n", label, val);
}

void test_symbolmap(void) {
  symbol_map_t *sm;
  address_t buf;
  int result;

  sm = smap_new(10);
  if (sm == NULL) {
      /* Handle allocation failure... */
  }
  /* Insert a couple of string associations */
  smap_put(sm, "application name", 1);
  smap_put(sm, "application version", 2);

  smap_enum(sm, &print_symbol, NULL);
  /* Retrieve a address */
  result = smap_get_address(sm, "application name", &buf);
  if (result == 0) {
      printf("KEY: %s Not found\n", "application name");
  }
  printf("KEY: %s, Value: %d\n", "application name", buf);

  /* When done, destroy the map object */
  smap_delete(sm);
}
