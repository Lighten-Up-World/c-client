#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "arm.h"
#include "register.h"

/* Takes as an argument the pointer to the register contents and register
 * address to print and outputs it's contents. */
void printReg(state_t* state, reg_address_t reg) {
  assert(reg >= 0 && reg < REG_N); /* only prints general purpose registers. */
  assert(state != NULL);
  if(reg == REG_N_LR || reg == REG_N_SP){
    return;
  }
  if(reg >= 0 && reg < NUM_GENERAL_REGISTERS){
    printf("$%-4u:", reg);
  }
  else if(reg == REG_N_PC){
    printf("PC  :");
  }
  else if(reg == REG_N_CPSR){
    printf("CPSR:");
  }
  printf("%11d (0x%8x)\n", getRegister(state, reg), getRegister(state, reg));
}


/* gets the 32 bit word from memory given a byte by shifting the 4 respective
 * bytes and OR-ing them together */
word_t getMemWord(state_t* state, int byteAddr) {
  assert(state != NULL);
  word_t word = 0;

  word |= state->memory[byteAddr] << 32;
  word |= state->memory[byteAddr+1] << 24;
  word |= state->memory[byteAddr+2] << 16;
  word |= state->memory[byteAddr+3] << 8;

  return word;
}


/* prints the values stored in memory until the instruction is 0 or we run
 * out of memory.  */
void printMem(state_t *state) {
  assert(state != NULL);
  word_t memWord;

  for (int addr = 0; addr < MEM_SIZE; addr+=4) {
    memWord = getMemWord(state, addr);
    if (memWord == 0) { // halt when memory instr is 0.
      break;
    }
    printf("0x%08x:     0x%08x\n", addr, memWord);
  }
}

void printPipeline(state_t* state);

void printState(state_t* state) {
  assert(state != NULL);

  printf("Registers:\n");
  for (int i = 0; i < REG_N; ++i) {
    printReg(state, i);
  }

  printf("Non-zero memory:");
  printMem(state);
}
