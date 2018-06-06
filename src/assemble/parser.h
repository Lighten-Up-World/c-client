#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "../assemble.h"
#include "../utils/instructions.h"

typedef struct {
  char *suffix;
  byte_t cond;
} branch_suffix_to_cond;

typedef struct {
  char *op;
  opcode_t op_enum;
  int (*parse_func) (program_t*, token_list_t*, instruction_t*);
} opcode_to_parser;

#define NUM_NON_BRANCH_OPS 16
#define NUM_BRN_SUFFIXES 8

int parse(program_t* prog, token_list_t *tlst, instruction_t *inst);

int parse_dp(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_mul(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_sdt(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_brn(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_lsl(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_halt(program_t* prog, token_list_t *tlst, instruction_t *inst);

#endif
