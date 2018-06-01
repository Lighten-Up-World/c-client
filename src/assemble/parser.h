#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "../utils/instructions.h"

int consume_token(token_t *arr, token_type_t type);

int parse(token_t *tkns, instruction_t *inst);

int parseDP(token_t *tkns, instruction_t *inst);
int parseMUL(token_t *tkns, instruction_t *inst);
int parseSDT(token_t *tkns, instruction_t *inst);
int parseBRN(token_t *tkns, instruction_t *inst);

#endif
