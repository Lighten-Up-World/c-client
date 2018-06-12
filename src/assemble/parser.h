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

// Definitions used in aux functions
#define MAX_ROT_VAL 16
#define HEX_BASE 16
#define DEC_BASE 10
#define MSB_24_START 31
#define MSB_24_END 8

// Definitions used in parse_dp
#define RD_POS 1 //re-used in the following instrs

// Definitions used in parse_mul
#define RM_POS 3
#define RS_POS 5
#define RN_POS 7
#define HEX_NINE 0x9

// Definitions used in parse_sdt
#define NUM_TOKS_EQ_EXPR 4
#define MAX_HEX 0xFF
#define NUM_TOKS_POST_IND_ADDR 6
#define NUM_TOKS_HASH_EXPR 8

// Definitions used in parse
#define AL_COND_CODE 0xE

int parse(program_t* prog, token_list_t *tlst, instruction_t *inst);

int parse_dp(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_mul(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_sdt(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_brn(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_lsl(program_t* prog, token_list_t *tlst, instruction_t *inst);
int parse_halt(program_t* prog, token_list_t *tlst, instruction_t *inst);

#endif
