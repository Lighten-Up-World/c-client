#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "./utils/arm.h"
#include "assemble/symbolmap.h"
#include "assemble/referencemap.h"
#include "assemble/map.h"

#define LINE_SIZE 511

typedef struct {
  byte_t out[MEM_SIZE];
  symbol_map_t *sym_m;
  reference_map_t *ref_m;
  char **in;
  int lines;
  word_t mPC;
} program_t;

program_t *program_new();
int program_delete(program_t *);
int program_add_symbol(program_t *, label_t, address_t);
int program_add_reference(program_t *, label_t, address_t);
void program_toString(program_t *, char *);

#endif
