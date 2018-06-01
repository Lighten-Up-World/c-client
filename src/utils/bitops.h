#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

flag_t get_flag(word_t inst, byte_t pos);
byte_t get_byte(word_t inst, byte_t pos);
byte_t get_nibble(word_t inst, byte_t pos);
word_t get_bits(word_t inst, byte_t x, byte_t y);

word_t l_shift_left(word_t value, byte_t shift);
word_t l_shift_right(word_t value, byte_t shift);
word_t a_shift_right(word_t value, byte_t shift);
word_t rotate_right(word_t value, byte_t shift);
shift_result_t l_shift_left_c(word_t value, byte_t shift);
shift_result_t l_shift_right_c(word_t value, byte_t shift);
shift_result_t a_shift_right_c(word_t value, byte_t shift);
shift_result_t rotate_right_c(word_t value, byte_t shift);

flag_t is_negative(word_t word);
word_t negate(word_t word);
word_t left_pad_zeros(byte_t value);

#endif
