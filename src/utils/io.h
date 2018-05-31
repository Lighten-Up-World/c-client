#ifndef IO_H
#define IO_H

#include "arm.h"
#include "emulate/register.h"

int get_mem_word(state_t *state, word_t byteAddr, word_t *dest);
int get_mem_word_big_end(state_t *state, word_t byteAddr, word_t *dest);

int set_mem_word(state_t *state, word_t byteAddr, word_t word);

void print_reg(state_t *state, reg_address_t reg);
void print_mem(state_t *state);
void print_state(state_t *state);

int write_file(const char *path, byte_t *buffer, size_t buffer_size);
int read_file(const char *path, byte_t *buffer, size_t buffer_size);

#endif
