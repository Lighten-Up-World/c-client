#ifndef ENCODE_H
#define ENCODE_H

#include "../utils/arm.h"
#include "../utils/instructions.h"

///BIT GROUP SIZES

///General Instructions

#define FLAG_SIZE 1
#define REG_SIZE 4

///DP

#define OPCODE_SIZE 4
#define DP_PAD0_SIZE 2
#define OP_ROTATE_SIZE 4
#define OP_IMM_SIZE 8

#define OP_SHIFT_TYPE_SIZE 2
#define OP_SHIFT_INT_SIZE 5

///MUL

#define MUL_PAD0_SIZE 6
#define PAD9_SIZE 4

///SDT

#define SDT_OFFSET_SIZE 12
#define SDT_PAD1_SIZE 2
#define SDT_PAD0_SIZE 2

///BRN

#define BRN_PADA_SIZE 4
#define BRN_OFFSET_SIZE 24;

int encode(instruction_t *instr, word_t *w);
int encode_dp(instruction_t *instr, word_t *w);
int encode_mul(instruction_t *instr, word_t *w);
int encode_sdt(instruction_t *instr, word_t *w);
int encode_brn(instruction_t *instr, word_t *w);
int encode_hal(instruction_t *instr, word_t *w);

#endif
