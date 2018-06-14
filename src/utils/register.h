#ifndef REGISTER_H
#define REGISTER_H

#define REG_N 17
#define REG_N_SP 13
#define REG_N_LR 14
#define REG_N_PC 15
#define REG_N_CPSR 16
#define NUM_FLAGS 4

#include "../utils/arm.h"
#include "../emulate.h"

int is_valid_register(reg_address_t reg);

word_t get_register(emulate_state_t *state, reg_address_t reg);
void set_register(emulate_state_t *state, reg_address_t reg, word_t value);

word_t get_pc(emulate_state_t *state);
void set_pc(emulate_state_t *state, word_t value);
void increment_pc(emulate_state_t *state);

byte_t get_flags(emulate_state_t *state);
void set_flags(emulate_state_t *state, byte_t value);

#endif
