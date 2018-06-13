#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "../assemble.h"
#include "../utils/instructions.h"

typedef struct {
  char *suffix;
  byte_t num;
} suffix_to_num;

typedef struct {
  char *op;
  opcode_t op_enum;
  int (*parse_func) (assemble_state_t*, instruction_t*);
} opcode_to_parser;

#define NUM_NON_BRANCH_OPS 16
#define NUM_BRN_SUFFIXES 8
#define NUM_SHIFT_SUFFIXES 4

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
#define NUM_TOKS_PRE_IND_ADDR 6
#define NUM_TOKS_HASH_EXPR 8

// Definitions used in parse
#define AL_COND_CODE 0xE

int parse(assemble_state_t* prog, instruction_t *inst);

int parse_dp(assemble_state_t* prog, instruction_t *inst);
int parse_mul(assemble_state_t* prog, instruction_t *inst);
int parse_sdt(assemble_state_t* prog, instruction_t *inst);
int parse_brn(assemble_state_t* prog, instruction_t *inst);
int parse_lsl(assemble_state_t* prog, instruction_t *inst);
int parse_halt(assemble_state_t* prog, instruction_t *inst);
#endif
