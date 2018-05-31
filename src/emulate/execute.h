#ifndef EXECUTE_H
#define EXECUTE_H

#include "utils/arm.h"

#define OFFSET_BITMASK 0xFF000000

int condition(state_t *state, byte_t cond);
int execute(state_t *state);
int execute_dp(state_t *state, dp_instruction_t instr);
int execute_mul(state_t *state, mul_instruction_t instr);
int execute_brn(state_t *state, brn_instruction_t instr);
int execute_sdt(state_t *state, sdt_instruction_t instr);
int execute_halt(state_t *state);

#endif
