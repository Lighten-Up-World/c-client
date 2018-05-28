#ifndef MAP_H
#define MAP_H

#include "arm.h"
#include <stdio.h>
#include <string.h>

typedef char * label_t;
typedef void (*map_func_t) (const label_t label, const address_t val, const void *obj);

unsigned long hash(const label_t label);

#endif
