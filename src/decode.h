#ifndef ARM11_22_DECODE_H
#define ARM11_22_DECODE_H

#include "arm.h"
#include "instructions.h"

//The following define the bit ranges for specific bit groups in instructions

#define INSTR_TYPE_START 27
#define INSTR_TYPE_END 25

#define COND_START 31

#define A_FLAG 23
#define S_FLAG 20

#define REG_1_START 19

#define REG_2_START 15

#define REG_S_START 11

#define MUL_TYPE_START 7
#define MUL_TYPE_END 4

#define REG_M_START 3

instruction_t decodeWord(word_t word);

#endif