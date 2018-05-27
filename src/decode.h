#ifndef ARM11_22_DECODE_H
#define ARM11_22_DECODE_H

#include "arm.h"
#include "instructions.h"

//The following define the bit ranges for specific bit groups in instructions

#define COND_START 31
#define COND_END 28


#define INSTR_TYPE_START 27
#define INSTR_TYPE_END 25

//// DP DEFINITIONS ////
#define DP_I 25

#define OPCODE_START 24
#define OPCODE_END 21

#define DP_S 20

#define RN_START 19
#define RN_END 16

#define RD_START 15
#define RD_END 12

#define OP2_ROTATE_ST 11
#define OP2_ROTATE_END 8

#define OP2_IMM_START 7
#define OP2_IMM_END 0

#define SHIFT_SELECT 4

#define SHFT_VAL_START 11
#define SHIFT_VAL_END 7

#define RS_START 11
#define RS_END 8

#define SHIFT_TYPE_START 6
#define SHIFT_TYPE_END 5

#define OP2_RM_START 3
#define OP2_RM_END 0


//// MUL DEFINITIONS ////
#define MUL_ACC 23

#define MUL_SET 20

#define REG_1_START 19
#define REG_1_END 16

#define REG_2_START 15
#define REG_2_END 12

#define REG_S_START 11
#define REG_S_END 8

#define MUL_TYPE_START 7
#define MUL_TYPE_END 4

#define REG_M_START 3
#define REG_M_END 0


instruction_t decodeWord(word_t word);

#endif