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
  mnemonic_t op_enum;
  int (*parse_func) (token_t*, instruction_t*);
} mnemonic_to_parser;
const int NUM_NON_BRANCH_OPS = 16;
const int NUM_BRN_SUFFIXES = 8;
const branch_suffix_to_cond brn_suffixes[] = {
    {"eq", 0b0000},
    {"ne", 0b0001},
    {"ge", 0b1010},
    {"lt", 0b1011},
    {"gt", 0b1100},
    {"le", 0b1101},
    {"al", 0b1110},
    {"", 0b1110}
};
const mnemonic_to_parser oplist[] = {
    {"add", M_ADD, parse_dp},
    {"sub", M_SUB,  parse_dp},
    {"rsb", M_RSB, parse_dp},
    {"and", M_AND,  parse_dp},
    {"eor", M_TEQ,  parse_dp},
    {"orr", M_ORR,  parse_dp},
    {"mov", M_MOV,  parse_dp},
    {"tst", M_TST,  parse_dp},
    {"teq", M_TEQ,  parse_dp},
    {"cmp", M_CMP,  parse_dp},

    {"mul", M_MUL, parse_mul},
    {"mla", M_MLA, parse_mul},

    {"ldr", M_LDR, parse_sdt},
    {"str", M_STR, parse_sdt},

    {"beq", M_BEQ, parse_brn},
    {"bne", M_BNE, parse_brn},
    {"blt", M_BLT, parse_brn},
    {"bgt", M_BGT, parse_brn},
    {"ble", M_BLE, parse_brn},
    {"b", M_B, parse_brn},
    {"bal", M_B, parse_brn},

    {"lsl", M_LSL, parse_lsl},
    {"andeq", M_ANDEQ, parse_halt}
};

int consume_token(token_t *arr, token_type_t type);

int parse(token_t *tkns, instruction_t *inst, int tkn);

int parse_dp(token_t *tkns, instruction_t *inst);
int parse_mul(token_t *tkns, instruction_t *inst);
int parse_sdt(token_t *tkns, instruction_t *inst);
int parse_brn(token_t *tkns, instruction_t *inst);
int parse_lsl(token_t *tokens, instruction_t *inst);
int parse_halt(token_t *tokens, instruction_t *inst);

#endif
