#ifndef EXECUTE_H
#define EXECUTE_H

#include "arm.h"

#define OFFSET_BITMASK 0xFF000000

int condition(state_t *state, byte_t cond);

int execute(state_t *state);
int executeDP(state_t *state, dp_instruction_t   instr);
int executeMUL(state_t *state, mul_instruction_t instr);
int executeBRN(state_t *state, brn_instruction_t instr);
int executeSDT(state_t *state, sdt_instruction_t instr);
int executeHAL(state_t *state);

#endif
