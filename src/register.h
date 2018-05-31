#ifndef REGISTER_H
#define REGISTER_H

#define REG_N 17
#define REG_N_SP 13
#define REG_N_LR 14
#define REG_N_PC 15
#define REG_N_CPSR 16
#define NUM_FLAGS 4

#include "arm.h"

word_t getRegister(state_t *state, reg_address_t reg);
void setRegister(state_t *state, reg_address_t reg, word_t value);

word_t getPC(state_t *state);
void setPC(state_t *state, word_t value);
void incrementPC(state_t *state);

byte_t getFlags(state_t *state);
void setFlags(state_t *state, byte_t value);

#endif
