#ifndef ARM11_22_DECODE_H
#define ARM11_22_DECODE_H

#include "arm.h"
#include "instructions.h"

//The following define the bit ranges for specific bit groups in instructions

//// GENERAL INSTRUCTION DEFINITIONS ////

#define COND_START 31

#define INSTR_TYPE_START 27
#define INSTR_TYPE_END 25

#define A_FLAG 23
#define S_FLAG 20

#define I_FLAG 25
#define P_FLAG 24
#define U_FLAG 23
#define L_FLAG 20

//// DP DEFINITIONS ////

#define OPCODE_START 24

#define DP_RN_START 19

#define DP_RD_START 15


//// MUL DEFINITIONS ////
#define MUL_ACC 23

#define MUL_SET 20

#define MUL_RD_START 19

#define MUL_RN_START 15

#define REG_S_START 11

#define MUL_TYPE_START 7
#define MUL_TYPE_END 4

#define REG_M_START 3

#define OP_START 11
#define OP_IMM_START 7
#define OP_SHIFTBY_FLAG 4
#define OP_SHIFT_TYPE_START 6
#define OP_SHIFT_TYPE_END 5

//// SDT DEFINITIONS ////

#define SDT_RN_START 19

#define SDT_RD_START 15


instruction_t decodeWord(word_t word);


#endif
