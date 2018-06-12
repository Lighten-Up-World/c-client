#ifndef REGISTER_H
#define REGISTER_H

#define REG_N 17
#define REG_N_SP 13
#define REG_N_LR 14
#define REG_N_PC 15
#define REG_N_CPSR 16
#define NUM_FLAGS 4

#include "../utils/arm.h"

int is_valid_register(reg_address_t reg);

word_t get_register(state_t *state, reg_address_t reg);
void set_register(state_t *state, reg_address_t reg, word_t value);

word_t get_pc(state_t *state);
void set_pc(state_t *state, word_t value);
void increment_pc(state_t *state);

byte_t get_flags(state_t *state);
void set_flags(state_t *state, byte_t value);

#endif
