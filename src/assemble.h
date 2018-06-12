#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "./utils/arm.h"
#include "assemble/symbolmap.h"
#include "assemble/referencemap.h"

#define LINE_SIZE 512
#define MAX_NUM_LINES 50

typedef struct {
  byte_t out[MEM_SIZE];
  symbol_map_t *smap;
  reference_map_t *rmap;
  char **in;
  int lines;
  word_t mPC;
} program_state_t;

typedef struct {
  program_state_t *prog;
  label_t label;
  address_t addr;
} prog_collection_t;

#endif
