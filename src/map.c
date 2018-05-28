#include "map.h"

// Based on djb2 by Dan Bernstein
unsigned long hash(const label_t label){
  unsigned long hash = 5381;
  for (size_t i = 0; i < strlen(label); i++) {
    hash = ((hash << 5) + hash) + label[i];
  }
	return hash;
}
