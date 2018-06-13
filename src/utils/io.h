#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "arm.h"

#define GPIO_SETUP_0_9 0x20200000
#define GPIO_SETUP_10_19 0x20200004
#define GPIO_SETUP_20_29 0x20200008
#define GPIO_CLEAR 0x20200028
#define GPIO_WRITE 0x2020001C

int get_word(byte_t *buff, word_t byteAddr, word_t *word);
int get_mem_word(state_t *state, word_t byteAddr, word_t *dest);
int get_mem_word_big_end(state_t *state, word_t byteAddr, word_t *dest);

int set_word(byte_t *buff, word_t byteAddr, word_t word);
int set_mem_word(state_t *state, word_t byteAddr, word_t word);

void print_reg(state_t *state, reg_address_t reg);
void print_mem(state_t *state);
void print_state(state_t *state);

int write_file(const char *path, byte_t *buffer, int no_bytes);
int read_file(const char *path, byte_t *buffer, size_t buffer_size);
int read_char_file(const char *path, char ** buffer);

char *num_to_str(int n);

#endif
