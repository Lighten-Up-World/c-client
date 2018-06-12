/*
*  Contains IO related operations, operating on either the ARM machine state or local disk.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "io.h"
#include "bitops.h"
#include "register.h"
#include "error.h"

/**
 * Determine whether an address is a gpio key memory location
 *
 * @param addr: the address to check
 * @return true iff the address is a valid gpio address
 */
bool is_gpio_addr(word_t addr) {
  if (addr >= GPIO_SETUP_0_9 & addr <= GPIO_SETUP_20_29) {
    return true;
  } else if (addr == GPIO_CLEAR) {
    return true;
  } else if (addr == GPIO_WRITE) {
    return true;
  }
  return false;
}

/**
 * Print out the correct string for a GPIO memory access
 *
 * @param byteAddr: the address accessed
 */
void print_gpio_access(word_t byteAddr) {
  if (byteAddr == GPIO_SETUP_0_9) {
    printf("One GPIO pin from 0 to 9 has been accessed\n");
  } else if (byteAddr == GPIO_SETUP_10_19) {
    printf("One GPIO pin from 10 to 19 has been accessed\n");
  } else if (byteAddr == GPIO_SETUP_20_29) {
    printf("One GPIO pin from 20 to 29 has been accessed\n");
  } else if (byteAddr == GPIO_CLEAR) {
    printf("PIN OFF\n");
  } else if (byteAddr == GPIO_WRITE) {
    printf("PIN ON\n");
  }
}

int check_address_valid(word_t addr) {
  //GPIO extension
  if (is_gpio_addr(addr)) {
    return 0;
  }

  if (addr > MEM_SIZE) {
    printf("Error: Out of bounds memory access at address 0x%08x\n", addr);
    return 1;
  }
  return 0;
}

char *itoa(int n)
{
  char *res = calloc(8, sizeof(int));
  sprintf(res, "%d", n);
  return res;
}

/**
 *  Read a 32 bit word from memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @param byteAddr: the byte address to read from
 *  @param dest: a non-null pointer to destination of loaded word
 *  @return an int indicating success or failure
 */
int get_mem_word(state_t *state, word_t byteAddr, word_t *dest) {
  assert(state != NULL);
  word_t word = 0;
  if (check_address_valid(byteAddr)) {
    return 1;
  }

  // GPIO extension
  if (is_gpio_addr(byteAddr)) {
    print_gpio_access(byteAddr);
    *dest = byteAddr;
    return 0;
  }

  for (size_t i = 0; i < 4; i++) {
    word |= ((word_t) state->memory[byteAddr + i]) << (i * 8);
  }
  *dest = word;
  return 0;
}

/**
 * Read the 32 bit word from memory and return it
 *
 * @param state - a non-null pointer to the machine state
 * @param byteAddr - byte address to read from.
 * @param dest: a non-null pointer to destination of loaded word
 * @return an int indicating success or failure
 */
int get_mem_word_big_end(state_t *state, word_t byteAddr, word_t *dest) {
  assert(state != NULL);
  word_t word = 0;
  if (check_address_valid(byteAddr)) {
    return 1;
  }

  // GPIO extension
  if (is_gpio_addr(byteAddr)) {
    print_gpio_access(byteAddr);
    *dest = byteAddr;
    return 0;
  }

  for (size_t i = 0; i < 4; i++) {
    word |= ((word_t) state->memory[byteAddr + 3 - i]) << (i * 8);
  }
  *dest = word;
  return 0;
}

int set_word(byte_t *buff, word_t byteAddr, word_t word){
  assert(buff != NULL);

  // GPIO extension
  if (is_gpio_addr(byteAddr)) {
    print_gpio_access(byteAddr);
    return EC_OK;
  }

  for (size_t i = 1; i < 5; i++) {
    DEBUG_PRINT("M[%04x] = %04x\n\t\t",
                byteAddr + (word_t) i - 1,
                get_byte(word, (i * 8) - 1));
    buff[byteAddr + i - 1] = get_byte(word, (i * 8) - 1);
  }
  return EC_OK;
}

/**
 * Write a word at a specified byte address in memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @param byteAddr: the byte address to be written into
 *  @param word: the word to write into memory
 *  @return: return 0 iff success
 */
int set_mem_word(state_t *state, word_t byteAddr, word_t word) {
  assert(state != NULL);
  if (check_address_valid(byteAddr)) { return EC_INVALID_PARAM; }
  return set_word(state->memory, byteAddr, word);
}

/**
 *  Print the values stored in a specified register
 *
 *  @param state: a non-null pointer to the machine state
 *  @param reg: the address of the register to print
 *  @return void
 */
void print_reg(state_t *state, reg_address_t reg) {
  assert(reg >= 0 && reg < REG_N);
  assert(state != NULL);
  if (reg == REG_N_LR || reg == REG_N_SP) {
    return;
  }
  if (reg >= 0 && reg < NUM_GENERAL_REGISTERS) {
    printf("$%-3u:", reg);
  } else if (reg == REG_N_PC) {
    printf("PC  :");
  } else if (reg == REG_N_CPSR) {
    printf("CPSR:");
  }
  printf("%11d (0x%08x)\n", get_register(state, reg), get_register(state,
                                                                   reg));
}

/**
 *  Print the values stored in memory
 *  Continue until the word value is 0 or we run out of memory
 *
 *  @param state: a non-null pointer to the machine state
 *  @return void
 */
void print_mem(state_t *state) {
  assert(state != NULL);
  word_t memWord;

  for (int addr = 0; addr < MEM_SIZE; addr += 4) {
    get_mem_word_big_end(state, addr, &memWord);
    if (memWord == 0) { // halt when memory instr is 0.
      continue;
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
void print_state(state_t *state) {
  assert(state != NULL);

  printf("Registers:\n");
  for (int i = 0; i < REG_N; ++i) {
    print_reg(state, i);
  }

  printf("Non-zero memory:\n");
  print_mem(state);
}

/**
 *  Write a file from a buffer to disk
 *
 *  @param path: the path of the binary file to write to
 *  @param buffer: a pointer to an allocated array in which the file will be written from
 *  @param buffer_size: the size of the buffer allocated
 *  @return a status code denoting the result
 */
int write_file(const char *path, byte_t *buffer, size_t buffer_size) {
  FILE *fp = fopen(path, "wb");
  if (fp == NULL) {
    perror("fopen failed at path");
    return 1;
  }
  const int read = fwrite(buffer, buffer_size, 1, fp);
  if (read != buffer_size && ferror(fp)) {
    perror("Couldn't write file to completion");
    return 3;
  }
  if (fclose(fp) != 0) {
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
int read_file(const char *path, byte_t *buffer, size_t buffer_size) {
  long file_size = 0;
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    perror("fopen failed at path");
    return 1;
  }
  file_size = ftell(fp);
  if (file_size == -1) {
    perror("Couldn't determine file size");
    return 2;
  }
  const int read = fread(buffer, buffer_size, 1, fp);
  if (read != file_size && ferror(fp)) {
    perror("Couldn't read file to completion");
    return 3;
  }
  if (fclose(fp) != 0) {
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

/**
*  Loads a file from disk as list of strings
*
*  @param path: the path of the ASCII file to read from
*  @param buffer: a pointer to an allocated array which the file will be read to
*  @param buffer_size: the size of the buffer allocated
*  @param num_of_lines: pointer to an int denoting the number of lines found in file
*  @return a status code denoting the result
*/
int read_char_file(const char *path, char ** buffer, int* num_of_lines) {
  FILE *fp = fopen(path, "r");
  if (fp == NULL || fp == 0) {
    perror("fopen failed at path");
    return 1;
  }
  int line = 0;
  size_t lineLength = 512 * sizeof(char);
  while (fgets(buffer[line], lineLength, fp) != NULL){
    DEBUG_PRINT("%s\n", buffer[line]);
    line++;
  }
  if (ferror(fp)){
    perror("fget failed");
    return 2;
  }

  if (fclose(fp) != 0) {
    perror("Couldn't close file");
    return 4;
  }
  *num_of_lines = line;
  return 0;
}
