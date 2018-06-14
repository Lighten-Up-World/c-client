#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>
#include <stdlib.h>
#include "../utils/bitops.h"
#include "../utils/register.h"
#include "../utils/io.h"
#include "../utils/instructions.h"
#include "../utils/arm.h"
#include "../emulate.h"
#include "decode.h"

#define OFFSET_BITMASK 0xFF000000

int condition(emulate_state_t *state, byte_t cond);
int execute(emulate_state_t *state);
int execute_dp(emulate_state_t *state, dp_instruction_t instr);
int execute_mul(emulate_state_t *state, mul_instruction_t instr);
int execute_brn(emulate_state_t *state, brn_instruction_t instr);
int execute_sdt(emulate_state_t *state, sdt_instruction_t instr);
int execute_halt(emulate_state_t *state);

#endif
