#ifndef IO_H
#define IO_H

#include "arm.h"
#include "register.h"

word_t getMemWord(state_t* state, address_t byteAddr);
word_t getMemWordBigEnd(state_t* state, address_t byteAddr);

int setMemWord(state_t* state, address_t byteAddr, word_t word);

void printReg(state_t* state, reg_address_t reg);
void printMem(state_t *state);
void printState(state_t* state);

int writeFile(const char *path, byte_t *buffer, size_t buffer_size);
int readFile(const char *path, byte_t *buffer, size_t buffer_size);

#endif
