#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "io.h"

/* gets the 32 bit word from memory given a byte by shifting the 4 respective
 * bytes and OR-ing them together */
word_t getMemWord(state_t* state, int byteAddr) {
  assert(state != NULL);
  word_t word = 0;

  for (size_t i = 0; i < 4; i++){
    word |= ((word_t) state->memory[byteAddr + i]) << (i * 8);
  }

  return word;
}

int setMemWord(state_t* state, int byteAddr, word_t word){
  assert(state != NULL);
  for (size_t i = 1; i < 5; i++){
    state->memory[byteAddr + i]) = getByte(word, (i * 8));
  }
  return 0;
}

/* Takes as an argument the pointer to the register contents and register
 * address to print and outputs it's contents. */
void printReg(state_t* state, reg_address_t reg) {
  assert(reg >= 0 && reg < REG_N); /* only prints general purpose registers. */
  assert(state != NULL);
  if(reg == REG_N_LR || reg == REG_N_SP){
    return;
  }
  if(reg >= 0 && reg < NUM_GENERAL_REGISTERS){
    printf("$%-3u:", reg);
  }
  else if(reg == REG_N_PC){
    printf("PC  :");
  }
  else if(reg == REG_N_CPSR){
    printf("CPSR:");
  }
  printf("%11d (0x%08x)\n", getRegister(state, reg), getRegister(state, reg));
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
    printf("0x%08x: 0x%08x\n", addr, memWord);
  }
}

void printState(state_t* state) {
  assert(state != NULL);

  printf("Registers:\n");
  for (int i = 0; i < REG_N; ++i) {
    printReg(state, i);
  }

  printf("Non-zero memory:\n");
  printMem(state);
}

/**
* Writes the entire file into the buffer (or throws an error)
*
* @param path - A string to the binary file to read.
* @param buffer - A pointer to an allocated array which it gets stored
* @param buffer_size - the size of buffer allocated.
* @return a status code for the result
*/
int writeFile(const char *path, byte_t *buffer, size_t buffer_size){
  FILE* fp = fopen(path, "wb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  const int read = fwrite(buffer, buffer_size, 1, fp);
  if(read != buffer_size && ferror(fp)){
    perror("Couldn't write file to completion");
    return 3;
  }
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

/**
* Loads the entire file into the buffer (or throws an error)
*
* @param path - A string to the binary file to read.
* @param buffer - A pointer to an allocated array which it gets stored
* @param buffer_size - the size of buffer allocated.
* @return a status code for the result
*/
int readFile(const char *path, byte_t *buffer, size_t buffer_size){
  long file_size = 0;
  FILE* fp = fopen(path, "rb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  file_size = ftell(fp);
  if(file_size == -1){
    perror("Couldn't determine file size");
    return 2;
  }
  const int read = fread(buffer, buffer_size, 1, fp);
  if(read != file_size && ferror(fp)){
    perror("Couldn't read file to completion");
    return 3;
  }
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

// int main(void){
//   state_t *state = calloc(1, sizeof(state_t));
//   if(state == NULL){
//     return EXIT_FAILURE;
//   }
//   readFile("../test/test_cases/add01", state->memory, MEM_SIZE);
//   printState(state);
// }
