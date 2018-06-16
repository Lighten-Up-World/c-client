/*
 * Contains functions for getting/setting registers as well as PC and flags.
 */

#include <stdio.h>
#include "../utils/arm.h"
#include "register.h"

/**
 *
 * @param reg:
 * @return:
 */
int is_valid_register(reg_address_t reg) {
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    return 1;
  }
  if (reg == REG_N_PC) {
    return 1;
  }
  if (reg == REG_N_CPSR) {
    return 1;
  }
  return 0;
}

/**
 *  Get the value of a register
 *
 *  @param state: non-null pointer to the machine state
 *  @param reg: register's address
 *  @return: word stored at that register
 */
word_t get_register(emulate_state_t *state, reg_address_t reg) {
  if (!is_valid_register(reg)) {
    return EC_INVALID_PARAM;
  }
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    return state->registers.r[reg];
  }
  if (reg == REG_N_SP || reg == REG_N_LR) {
    return EC_UNSUPPORTED_OP;
  }
  if (reg == REG_N_PC) {
    return state->registers.pc;
  }
  if (reg == REG_N_CPSR) {
    return state->registers.cpsr;
  }
  return EC_INVALID_PARAM;
}

/**
 *  Set the value of a register
 *
 *  @param state: non-null pointer to the machine state
 *  @param reg: register's address
 */
int set_register(emulate_state_t *state, reg_address_t reg, word_t value) {
  if (!is_valid_register(reg)) {
    return EC_INVALID_PARAM;
  }
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    state->registers.r[reg] = value;
  }
  if (reg == REG_N_SP || reg == REG_N_LR) {
    return EC_UNSUPPORTED_OP;
  }
  if (reg == REG_N_PC) {
    state->registers.pc = value;
    return EC_OK;
  }
  if (reg == REG_N_CPSR) {
    state->registers.cpsr = value;
    return EC_OK;
  }
  return EC_INVALID_PARAM;
}

/**
 *  Get the value of the PC
 *
 *  @param state: non-null pointer to the machine state
 *  @return: word stored in the PC
 */
word_t get_pc(emulate_state_t *state) {
  return get_register(state, REG_N_PC);
}

/**
 *  Set the value of the PC
 *
 *  @param state: a non-null pointer to the machine state
 *  @param value: the value to set the PC to
 */
void set_pc(emulate_state_t *state, word_t value) {
  set_register(state, REG_N_PC, value);
}

/**
 *  Increment the PC
 *
 *  @param state: non-null pointer to the machine state
 */
void increment_pc(emulate_state_t *state) {
  set_register(state, REG_N_PC, get_pc(state) + 4);
}

/**
 *  Get the flags from the flag register
 *
 *  @param state: non-null pointer to the machine state
 *  @return: byte with the 4 least significant bits set to the value of the
 *  flags
 */
byte_t get_flags(emulate_state_t *state) {
  return (get_register(state, REG_N_CPSR) >> (sizeof(word_t) * 8 - NUM_FLAGS));
}

/**
 *  Set the flags in the flag register
 *
 *  @param state: non-null pointer to the machine state
 *  @param value: byte with the 4 least significant bits set to the flag values
 */
void set_flags(emulate_state_t *state, byte_t value) {
  set_register(state, REG_N_CPSR, value << (sizeof(word_t) * 8 - NUM_FLAGS));
}
