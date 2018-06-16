#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "utils/arm.h"
#include "assemble/symbolmap.h"
#include "assemble/referencemap.h"
#include "utils/list.h"

#define LINE_SIZE 512
#define MAX_LINES 100

typedef struct {
  word_t word;
  address_t ref;
} wordref_t;

typedef struct {
  byte_t *out;
  symbol_map_t *smap;
  reference_map_t *rmap;
  list_t *additional_words;
  list_t *tklst;
  char **in;
  int lines;
  word_t mPC;
} assemble_state_t;

typedef struct {
  assemble_state_t *prog;
  label_t label;
  address_t addr;
} prog_collection_t;

#endif
