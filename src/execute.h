#ifndef EXECUTE_H
#define EXECUTE_H

#include "arm.h"

#define OFFSET_BITMASK 0xFF000000

int condition(state_t *state, byte_t cond);

void execute(state_t *state);
void executeDP(state_t *state, dp_instruction_t   instr);
void executeMUL(state_t *state, mul_instruction_t instr);
void executeBRN(state_t *state, brn_instruction_t instr);
void executeSDT(state_t *state, sdt_instruction_t instr);
void executeHAL(state_t *state);

#endif
