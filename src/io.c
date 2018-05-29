/*
 *  Contains IO related operations, operating on either the ARM machine state or local disk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "io.h"
#include "bitops.h"

/**
 *  Read a 32 bit word from memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @param byteAddr: the byte address to read from
 *  @return a word from memory at the given byte address
 */
word_t getMemWord(state_t* state, int byteAddr) {
  assert(state != NULL);
  word_t word = 0;

  for (size_t i = 0; i < 4; i++){
    word |= ((word_t) state->memory[byteAddr + i]) << (i * 8);
  }

  return word;
}

/**
 * Read the 32 bit word from memory and return it
 *
 * @param state - pointer to the state of the ARM machine.
 * @param byteAddr - byte address to read from.
 * @return word_t word - word read from memory at byte address.
 */
word_t getMemWordBigEnd(state_t* state, int byteAddr) {
  assert(state != NULL);
  word_t word = 0;

  for (size_t i = 0; i < 4; i++){
    word |= ((word_t) state->memory[byteAddr + 3 - i]) << (i * 8);
  }

  return word;
}

/**
 * Write a word at a specified byte address in memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @param byteAddr: the byte address to be written into
 *  @param word: the word to write into memory
 *  @return: return 0 iff success
 */
int setMemWord(state_t* state, int byteAddr, word_t word){
  assert(state != NULL);
  for (size_t i = 1; i < 5; i++){
    state->memory[byteAddr + i] = getByte(word, (i * 8));
  }
  return 0;
}

/**
 *  Print the values stored in a specified register
 *
 *  @param state: a non-null pointer to the machine state
 *  @param reg: the address of the register to print
 *  @return void
 */
void printReg(state_t* state, reg_address_t reg) {
  assert(reg >= 0 && reg < REG_N);
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

/**
 *  Print the values stored in memory
 *  Continue until the word value is 0 or we run out of memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @return void
 */
void printMem(state_t *state) {
  assert(state != NULL);
  word_t memWord;

  for (int addr = 0; addr < MEM_SIZE; addr+=4) {
<<<<<<< HEAD
    memWord = getMemWordBigEnd(state, addr);
    if (memWord == 0) { // halt when memory instr is 0.
=======
    memWord = getMemWord(state, addr);

    // Halt when word read is 0
    if (memWord == 0) {
>>>>>>> db895549cda43060e17fa618bfcde0d30dd22e55
      break;
    }
    printf("0x%08x: 0x%08x\n", addr, memWord);
  }
}

/**
 *  Print all data stored in registers and memory
 *
 *  @param state - a pointer to the state of the ARM machine
 *  @return void
 */
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
 *  Write a file from a buffer to disk
 *
 *  @param path: the path of the binary file to write to
 *  @param buffer: a pointer to an allocated array in which the file will be written from
 *  @param buffer_size: the size of the buffer allocated
 *  @return a status code denoting the result
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
*  Loads a file from disk into a buffer
*
*  @param path: the path of the binary file to read from
*  @param buffer: a pointer to an allocated array which the file will be read to
*  @param buffer_size: the size of the buffer allocated
*  @return a status code denoting the result
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
