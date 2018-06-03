#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "../utils/instructions.h"

int consume_token(token_t *arr, token_type_t type);

int parse(token_t *tkns, instruction_t *inst, int tkn);

int parse_dp(token_t *tkns, instruction_t *inst);
int parse_mul(token_t *tkns, instruction_t *inst);
int parse_sdt(token_t *tkns, instruction_t *inst);
int parse_brn(token_t *tkns, instruction_t *inst);
int parse_lsl(token_t *tokens, instruction_t *inst);
int parse_halt(token_t *tokens, instruction_t *inst);

#endif
