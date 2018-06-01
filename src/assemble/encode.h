#ifndef ENCODE_H
#define ENCODE_H

#include "../utils/arm.h"
#include "../utils/instructions.h"

int encode(instruction_t *instr, word_t *word);
// int encodeCond(instruction_t *instr, word_t *word);
int encode_dp(instruction_t *instr, word_t *word);
int encode_mul(instruction_t *instr, word_t *word);
int encode_sdt(instruction_t *instr, word_t *word);
int encode_brn(instruction_t *instr, word_t *word);
int encode_hal(instruction_t *instr, word_t *word);
#endif
