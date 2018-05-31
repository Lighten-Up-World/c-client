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

const uint8_t U_ONE = 1;

/**
 *  Get the bit at a given position, return it as a boolean flag
 *
 *  @param inst: the instruction to get flag from
 *  @param pos: the position of the flag
 *  @return a flag that is true iff the bit at pos is not 0
 */
flag_t getFlag(word_t inst, byte_t pos) {
 return (inst & ( 1 << pos )) >> pos;
}

/**
 *  Get the byte at a given position
 *
 *  @param inst: the instruction to get byte from
 *  @param pos: the start of the byte
 *  @return a byte containing the 8 bits downwards from pos, in inst
 */
byte_t getByte(word_t inst, byte_t pos) {
  assert(pos >= 7);
  return (inst >> (pos - 7)) & 0x000000ff;
}

/**
 *  Get the nibble at a given position (left pad by cast to byte_t)
 *
 *  @param inst: the instruction to get nibble from
 *  @param pos: the start of the nibble
 *  @return a byte containing the 4 bits downwards from pos, in inst
 */
byte_t getNibble(word_t inst, byte_t pos) {
  assert(pos >= 3);
  return (inst >> (pos - 3)) & 0x0000000f;
}

/**
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *  Limits are inclusive.
 *
 *  @param inst: the instruction to get bits from
 *  @param x: the MSb of interval to return
 *  @param y: the LSb of interval to return
 *  @return a word containing the specified bits, right aligned
 */
word_t getBits(word_t inst, byte_t x, byte_t y) {
  assert(x <= 31);
  assert(x >= y);
  assert(y >= 0);
  assert(!(x==31 && y==0));

  return (inst >> y) & ~(~(word_t)0 << (x + 1 - y));
}

/**
 *  Logical shift left with carry
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return a pair containing the shifted value and carry
 */
shift_result_t lShiftLeftC(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = lShiftLeft(value, shift);
  res.carry = 0;

  if (shift != 0) {
    res.carry = (flag_t) (value >> (sizeof(word_t) * 8 - shift)) & U_ONE;
  }
  return res;
}

/**
 *  Logical shift right with carry
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return a pair containing the shifted value and carry
 */
shift_result_t lShiftRightC(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = lShiftRight(value, shift);
  res.carry = (value << (sizeof(word_t) * 8 - shift)) & 0x80000000;

  return res;
}

/**
 *  Arithmetic shift right with carry
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return a pair containing the shifted value and carry
 */
shift_result_t aShiftRightC(word_t value, byte_t shift) {
  assert(shift >= 0);

  shift_result_t res;
  res.value = aShiftRight(value, shift);
  res.carry = (value << (sizeof(word_t) * 8 - shift)) & 0x80000000;

  return res;
}

/**
 *  Rotate right
 *
 *  @param value: the value to shift
 *  @param rotate: the amount to rotate by
 *  @return a pair containing the shifted value and carry
 */
shift_result_t rotateRightC(word_t value, byte_t rotate) {
  assert(rotate >= 0);

  shift_result_t res;
  res.value = rotateRight(value, rotate);
  res.carry = (value << (sizeof(word_t) * 8 - rotate)) & 0x80000000;

  return res;
}

/**
 *  Logical shift left
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return the shifted value
 */
word_t lShiftLeft(word_t value, byte_t shift) {
  assert(shift >= 0);
  return value << shift;
}

/**
 *  Logical shift right
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return the shifted value
 */
word_t lShiftRight(word_t value, byte_t shift) {
  assert(shift >= 0);
  return value >> shift;
}

/**
 *  Arithmetic shift right
 *
 *  @param value: the value to shift
 *  @param shift: the amount to shift by
 *  @return the shifted value
 */
word_t aShiftRight(word_t value, byte_t shift) {
  assert(shift >= 0);

  word_t msb = value >> (sizeof(word_t) * 8 - 1);

  if (msb == 0) {
    return lShiftRight(value, shift);
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
 *  @param value: the value to shift
 *  @param rotate: the amount to rotate by
 *  @return the shifted value
 */
word_t rotateRight(word_t value, byte_t rotate) {
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
 *  Checks if a 2s complement word is negative
 *
 *  @param word: the value to check the sign of
 *  @returns a flag that is set iff value is negative in 2s complement
 */
flag_t isNegative(word_t word){
  return (flag_t) lShiftRight(word, 31);
}

/**
 *  Negates a 2s complement word
 *
 *  @param word: the value to be negated
 *  @returns the negated word
 */
word_t negate(word_t word){
  return (~word) + 1;
}

/**
 *  Pad out a byte value to a word value, with zeros
 *
 *  @param value: the value to zero extend
 *  @return the zero extended value
 */
word_t leftPadZeros(byte_t value) {
  return value;
}
