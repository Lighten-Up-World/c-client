#include 'arm.h'
#include 'register.h'

#define REG_N_SP 13
#define REG_N_LR 14
#define REG_N_PC 15
#define REG_N_CPSR 16

/**
* Gets a register from its register number.
*
* @param state: A non-null state_t pointer
* @param reg: A register address
* @return The word stored at that register.
*/
word_t getRegister(state_t *state, reg_address_t reg){
  if(reg >= 0 && reg < NUM_GENERAL_REGISTERS){
    return state->registers.r[reg];
  }
  if(reg == REG_N_SP || reg == REG_N_LR){
    //TODO: Unsupported Operation
    return 0;
  }
  if(reg == REG_N_PC){
    return state->registers.pc;
  }
  if(reg == REG_N_CPSR){
    return state->registers.cpsr;
  }
  //TODO: Throw error for out of bounds register
  return 0;
}
/**
* Sets a register from its register number.
*
* @param state: A non-null state_t pointer
* @param reg: A register address
* @return The word stored at that register.
*/
void setRegister(state_t *state, reg_address_t reg, word_t value){
  if(reg >= 0 && reg < NUM_GENERAL_REGISTERS){
    state->registers.r[reg] = value;
  }
  if(reg == REG_N_SP || reg == REG_N_LR){
    //TODO: Unsupported Operation
    return;
  }
  if(reg == REG_N_PC){
    state->registers.pc = value;
  }
  if(reg == REG_N_CPSR){
    state->registers.cpsr = value;
  }
  //TODO: Throw error for out of bounds register
  return;
}

/**
* Convenience function for getting PC
*
* @param state: A non-null state_t pointer
* @return The word stored in PC.
*/
word_t getPC(state_t *state){
  return getRegister(state, REG_N_PC);
}
/**
* Convenience function for setting PC
*
* @param state: A non-null state_t pointer
* @param value: A word_t which PC is set to
*/
void setPC(state_t *state, word_t value){
  return setRegister(state, REG_N_PC, value);
}
/**
* Convenience function for incrementing PC
*
* @param state: A non-null state_t pointer
*/
void incrementPC(state_t *state){
  return setRegister(state, REG_N_PC, getPC(state)+4);
}
/**
* Convenience function for getting Flags
*
* @param state: A non-null state_t pointer
* @return A byte with the 4 LSB set to the flag values
*/
byte_t getFlags(state_t *state){
  return getRegister(state, REG_N_CPSR) >> (sizeof(word_t) - 4));
}
/**
* Convenience function for setting Flags
*
* @param state: A non-null state_t pointer
* @param value: A byte with the 4 LSB set to the flag values;
*/
void setFlags(state_t *state, byte_t value){
  return setRegister(state, REG_N_CPSR, value << (sizeof(word_t) - 4));
}
