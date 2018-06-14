/*
 *  All bit operations that may be needed can be found in here.
 *  This includes register shift operations, masking, and negating.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <printf.h>
#include "bitops.h"
#include "arm.h"

#define U_ONE ((uint8_t) 1)

/**
 *  Get the bit at a given position, return it as a boolean flag
 *
 *  @param inst: instruction to get flag from
 *  @param pos: position of the flag
 *  @return: flag that is true iff the bit at pos is not 0
 */
flag_t get_flag(word_t inst, byte_t pos) {
  return (inst & (1 << pos)) >> pos;
}

/**
 *  Get the byte at a given position
 *
 *  @param inst: instruction to get byte from
 *  @param pos: start of the byte
 *  @return: byte containing the 8 bits downwards from pos, in inst
 */
byte_t get_byte(word_t inst, byte_t pos) {
  assert(pos >= 7);
  return (inst >> (pos - 7)) & 0x000000ff;
}

/**
 *  Get the nibble at a given position (left pad by cast to byte_t)
 *
 *  @param inst: instruction to get nibble from
 *  @param pos: start of the nibble
 *  @return: byte containing the 4 bits downwards from pos, in inst
 */
byte_t get_nibble(word_t inst, byte_t pos) {
  assert(pos >= 3);
  return (inst >> (pos - 3)) & 0x0000000f;
}

/**
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *  Limits are inclusive.
 *
 *  @param inst: instruction to get bits from
 *  @param x: MSb of interval to return
 *  @param y: LSb of interval to return
 *  @return: word containing the specified bits, right aligned
 */
word_t get_bits(word_t inst, byte_t x, byte_t y) {
  assert(x <= 31);
  assert(x >= y);
  assert(y >= 0);
  assert(!(x == 31 && y == 0));

  return (inst >> y) & ~(~(word_t) 0 << (x + 1 - y));
}

/**
 *  Logical shift left with carry
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: pair containing the shifted value and carry
 */
shift_result_t l_shift_left_c(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = l_shift_left(value, shift);
  res.carry = 0;

  if (shift != 0) {
    res.carry = (flag_t) (value >> (sizeof(word_t) * 8 - shift)) & U_ONE;
  }
  return res;
}

/**
 *  Logical shift right with carry
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: pair containing the shifted value and carry
 */
shift_result_t l_shift_right_c(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = l_shift_right(value, shift);
  res.carry = (value << (sizeof(word_t) * 8 - shift)) & 0x80000000;

  return res;
}

/**
 *  Arithmetic shift right with carry
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: pair containing the shifted value and carry
 */
shift_result_t a_shift_right_c(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = a_shift_right(value, shift);
  res.carry = (value << (sizeof(word_t) * 8 - shift)) & 0x80000000;

  return res;
}

/**
 *  Rotate right
 *
 *  @param value: value to shift
 *  @param rotate: amount to rotate by
 *  @return: pair containing the shifted value and carry
 */
shift_result_t rotate_right_c(word_t value, byte_t rotate) {
  assert(rotate >= 0);

  shift_result_t res;
  res.value = rotate_right(value, rotate);
  res.carry = (value << (sizeof(word_t) * 8 - rotate)) & 0x80000000;

  return res;
}

/**
 *  Logical shift left
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: shifted value
 */
word_t l_shift_left(word_t value, byte_t shift) {
  assert(shift >= 0);
  return value << shift;
}

/**
 *  Logical shift right
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: shifted value
 */
word_t l_shift_right(word_t value, byte_t shift) {
  assert(shift >= 0);
  return value >> shift;
}

/**
 *  Arithmetic shift right
 *
 *  @param value: value to shift
 *  @param shift: amount to shift by
 *  @return: shifted value
 */
word_t a_shift_right(word_t value, byte_t shift) {
  assert(shift >= 0);

  word_t msb = value >> (sizeof(word_t) * 8 - 1);

  if (msb == 0) {
    return l_shift_right(value, shift);
  }

  word_t msbOnly = msb << ((sizeof(word_t) * 8) - 1);
  for (int i = 0; i < shift; i++) {
    value = value >> U_ONE;
    value = value | msbOnly;
  }

  return value;
}

/**
 *  Rotate right
 *
 *  @param value: value to rotate
 *  @param rotate: amount to rotate by
 *  @return: rotated value
 */
word_t rotate_right(word_t value, byte_t rotate) {
  assert(rotate >= 0);

  word_t lsb;
  for (int i = 0; i < rotate; i++) {
    lsb = value & U_ONE;
    value = value >> U_ONE;
    value = value | (lsb << ((sizeof(word_t) * 8) - 1));
  }

  return value;
}

/**
 *  Rotate left
 *
 *  @param value: value to rotated
 *  @param rotate: amount to rotate by
 *  @return: rotated value
 */
word_t rotate_left(word_t value, byte_t rotate) {
  assert(rotate >= 0);
  word_t msb_as_lsb;
  for (int i = 0; i < rotate; i++) {
    msb_as_lsb = value >> (sizeof(word_t) * 8 - 1);
    value = value << U_ONE;
    value = value | msb_as_lsb;
  }
  return value;
}

/**
 *  Check if a 2s complement word is negative
 *
 *  @param word: value to check the sign of
 *  @return: flag that is set iff value is negative in 2s complement
 */
flag_t is_negative(word_t word) {
  return (flag_t) l_shift_right(word, 31);
}

/**
 *  Negate a 2s complement word
 *
 *  @param word: value to be negated
 *  @return: negated word
 */
word_t negate(word_t word) {
  return (~word) + 1;
}

/**
 *  Pad out a byte value to a word value, with zeros
 *
 *  @param value: value to zero extend
 *  @return: zero extended value
 */
word_t left_pad_zeros(byte_t value) {
  return value;
}
