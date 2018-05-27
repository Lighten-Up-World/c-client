#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <printf.h>
#include "bitops.h"

const uint8_t U_ONE = 1;

/*
 *  Get the bit at a given position, return it as a boolean flag
 *
 *  @param inst: the instruction to get flag from
 *  @param pos: the position of the flag
 *  @return a flag_t that is true iff the bit at pos is not 0
 */
flag_t getFlag(word_t inst, byte_t pos) {
  return (flag_t) getBits(inst, pos, pos);
}

/*
 *  Get the byte at a given position
 *
 *  @param inst: the instruction to get byte from
 *  @param pos: the start of the byte
 *  @return a byte containing the 8 bits downwards from pos, in inst
 */
byte_t getByte(word_t inst, byte_t pos) {
  assert(pos >= 7);
  return (byte_t) getBits(inst, pos, (byte_t) (pos - 7));
}

/*
 *  Get the nibble at a given position (left pad by cast to byte_t)
 *
 *  @param inst: the instruction to get nibble from
 *  @param pos: the start of the nibble
 *  @return a byte containing the 4 bits downwards from pos, in inst
 */
byte_t getNibble(word_t inst, byte_t pos) {
  assert(pos >= 3);
  return (byte_t) getBits(inst, pos, (byte_t) (pos - 3));
}

/*
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

  // Logical shift right so that y is the base
  inst = lShiftRight(inst, y);

  // Mask so that everything after x is zero
  word_t mask = 1;
  for (int i = 0; i < (x - y); i++) {
    mask = mask << U_ONE;
    mask = mask | U_ONE;
  }

  return inst & mask;
}

/*
 *  Logical shift left with carry
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value and carry pair
 */
shift_result_t lShiftLeftC(word_t value, byte_t shift) {
  assert(shift >= 0);
  shift_result_t res = {value << shift,
                        (value >> (sizeof(word_t) - shift)) & 0x1};
  return res;
}

/*
 *  Logical shift right with carry
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value and carry pair
 */
shift_result_t lShiftRightC(word_t value, byte_t shift) {
  assert(shift >= 0);
  shift_result_t res = {value >> shift,
                        (value << (sizeof(word_t) - shift)) & 0x80000000};
  return res;
}

/*
 *  Arithmetic shift right with carry
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value and carry pair
 */
shift_result_t aShiftRightC(word_t value, byte_t shift) {
  assert(shift >= 0);
  word_t msb = value >> (sizeof(word_t) * 8 - 1);

  if (msb == 0) {
    return lShiftRightC(value, shift);
  }
  shift_result_t res;
  res.carry = (value << (sizeof(word_t) - shift)) & 0x80000000;
  word_t msbOnly = msb << ((sizeof(word_t) * 8) - 1);
  for (int i = 0; i < shift; i++) {
    value = value >> U_ONE;
    value = value | msbOnly;
  }
  res.value = value;
  return res;
}

/*
 *  Rotate right
 *
 *  @param value: The value to shift
 *  @param rotate: The amount to rotate by
 *  @return the shifted value and carry pair
 */
shift_result_t rotateRightC(word_t value, byte_t rotate) {
  assert(rotate >= 0);
  shift_result_t res;
  res.carry = (value << (sizeof(word_t) - rotate)) & 0x80000000;
  word_t lsb;
  for (int i = 0; i < rotate; i++) {
    lsb = value & U_ONE;
    value = value >> U_ONE;
    value = value | (lsb << ((sizeof(word_t) * 8) - 1));
  }
  res.value = value;
  return res;
}

/*
 *  Logical shift left
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t lShiftLeft(word_t value, byte_t shift) {
  assert(shift >= 0);
  return lShiftLeftC(value, shift).value;
}

/*
 *  Logical shift right
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t lShiftRight(word_t value, byte_t shift) {
  assert(shift >= 0);
  return lShiftRightC(value, shift).value;
}

/*
 *  Arithmetic shift right
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t aShiftRight(word_t value, byte_t shift) {
  assert(shift >= 0);
  return aShiftRightC(value, shift).value;
}

/*
 *  Rotate right
 *
 *  @param value: The value to shift
 *  @param rotate: The amount to rotate by
 *  @return the shifted value
 */
word_t rotateRight(word_t value, byte_t rotate) {
  assert(rotate >= 0);
  return rotateRightC(value, rotate).value;
}

/**
* Checks if 2s complement word is negative.
* @param word The value to check sign.
* @returns True iff value is negative in 2s complement
*/
flag_t isNegative(word_t word){
  return word >> 31;
}

/**
* Negates a 2s Complement word.
* @param word The value to be negated
* @returns The negated word
*/
word_t negate(word_t word){
  return (~word) + 1;
}
/*
 *  Pad out a byte value to a word value, with zeros
 *
 *  @param value: the value to zero extend
 *  @return the zero extended value
 */
word_t leftPadZeros(byte_t value) {
  return value;
}
