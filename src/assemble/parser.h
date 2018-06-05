#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "../utils/instructions.h"

typedef struct {
  char *suffix;
  byte_t cond;
} branch_suffix_to_cond;

typedef struct {
  char *op;
  opcode_t op_enum;
  int (*parse_func) (program_t*, token_t*, instruction_t*);
} opcode_to_parser;

const int NUM_NON_BRANCH_OPS = 16;
const int NUM_BRN_SUFFIXES = 8;

const branch_suffix_to_cond brn_suffixes[NUM_BRN_SUFFIXES] = {
    {"eq", 0b0000},
    {"ne", 0b0001},
    {"ge", 0b1010},
    {"lt", 0b1011},
    {"gt", 0b1100},
    {"le", 0b1101},
    {"al", 0b1110},
    {"", 0b1110}
};
const opcode_to_parser oplist[NUM_NON_BRANCH_OPS] = {
    {"add", ADD, parse_dp},
    {"sub", SUB,  parse_dp},
    {"rsb", RSB, parse_dp},
    {"and", AND,  parse_dp},
    {"eor", EOR,  parse_dp},
    {"orr", ORR,  parse_dp},
    {"mov", MOV,  parse_dp},
    {"tst", TST,  parse_dp},
    {"teq", TEQ,  parse_dp},
    {"cmp", CMP,  parse_dp},

    {"mul", MOV, parse_mul},   // Dummy value for opcode_t
    {"mla", MOV, parse_mul},   // ...

    {"ldr", MOV, parse_sdt},   // ...
    {"str", MOV, parse_sdt},   // ...

    {"lsl", MOV, parse_lsl},   // ...
    {"andeq", MOV, parse_halt} // ...
};

int consume_token(token_t *arr, token_type_t type);

int parse(program_t* prog, token_t *tkns, instruction_t *inst, int tkn);

int parse_dp(program_t* prog, token_t *tkns, instruction_t *inst);
int parse_mul(program_t* prog, token_t *tkns, instruction_t *inst);
int parse_sdt(program_t* prog, token_t *tkns, instruction_t *inst);
int parse_brn(program_t* prog, token_t *tkns, instruction_t *inst);
int parse_lsl(program_t* prog, token_t *tokens, instruction_t *inst);
int parse_halt(program_t* prog, token_t *tokens, instruction_t *inst);

#endif
