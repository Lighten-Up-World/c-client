/// FILE DESCRIPTION ///

#include <stdio.h>
#include "utils/arm.h"
#include "register.h"

/**
 *  Get the value of a register
 *
 *  @param state: a non-null pointer to the machine state
 *  @param reg: the register's address
 *  @return the word stored at that register
 */
word_t getRegister(state_t *state, reg_address_t reg) {
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    return state->registers.r[reg];
  }
  if (reg == REG_N_SP || reg == REG_N_LR) {
    //TODO: Unsupported Operation
    return 0;
  }
  if (reg == REG_N_PC) {
    return state->registers.pc;
  }
  if (reg == REG_N_CPSR) {
    return state->registers.cpsr;
  }
  //TODO: Throw error for out of bounds register
  return 0;
}

/**
 *  Set the value of a register
 *
 *  @param state: a non-null pointer to the machine state
 *  @param reg: the register's address
 *  @return void
 */
void setRegister(state_t *state, reg_address_t reg, word_t value) {
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    state->registers.r[reg] = value;
  }
  if (reg == REG_N_SP || reg == REG_N_LR) {
    //TODO: Unsupported Operation
    return;
  }
  if (reg == REG_N_PC) {
    state->registers.pc = value;
  }
  if (reg == REG_N_CPSR) {
    state->registers.cpsr = value;
  }
  //TODO: Throw error for out of bounds register
  return;
}

/**
 *  Get the value of the PC
 *
 *  @param state: a non-null pointer to the machine state
 *  @return the word stored in the PC
 */
word_t getPC(state_t *state) {
  return getRegister(state, REG_N_PC);
}

/**
 *  Set the value of the PC
 *
 *  @param state: a non-null pointer to the machine state
 *  @param value: the value to set the PC to
 */
void setPC(state_t *state, word_t value) {
  setRegister(state, REG_N_PC, value);
}

/**
 *  Increment the PC
 *
 *  @param state: a non-null pointer to the machine state
 */
void incrementPC(state_t *state) {
  setRegister(state, REG_N_PC, getPC(state) + 4);
}

/**
 *  Get the flags from the flag register
 *
 *  @param state: a non-null pointer to the machine state
 *  @return a byte with the 4 least significant bits set to the value of the flags
 */
byte_t getFlags(state_t *state) {
  return (getRegister(state, REG_N_CPSR) >> (sizeof(word_t) * 8 - NUM_FLAGS));
}

/**
 *  Set the flags in the flag register
 *
 *  @param state: a non-null pointer to the machine state
 *  @param value: a byte with the 4 least significant bits set to the flag values
 *  @return void
 */
void setFlags(state_t *state, byte_t value) {
  setRegister(state, REG_N_CPSR, value << (sizeof(word_t) * 8 - NUM_FLAGS));
}
