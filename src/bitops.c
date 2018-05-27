#include <stdlib.h>
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
  assert(31 >= x);
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
                        getFlag(value, sizeof(word_t) - shift - 1)};
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
                        getFlag(value, shift - 1)};
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
  res.carry = getFlag(value, shift - 1);
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
  res.carry = getFlag(value, rotate - 1);
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

// Main for testing
int main(int argc, char **argv) {

  word_t zero = 0;
  word_t five = 5;
  word_t sixtyThree = 63;
  word_t max = UINT32_MAX;
  word_t maxMSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 1);
  word_t max2MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 2);
  word_t max3MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 3);


  // Get bits
  assert(zero == getBits(zero, 0, 0));
  assert(zero == getBits(zero, 1, 0));
  assert(zero == getBits(zero, 17, 6));

  assert(five == getBits(five, 31, 0));
  assert(zero == getBits(five, 31, 21));
  assert(2 == getBits(five, 2, 1));
  assert(1 == getBits(five, 2, 2));

  assert(sixtyThree == getBits(sixtyThree, 31, 0));
  assert(zero == getBits(sixtyThree, 31, 29));
  assert(15 == getBits(sixtyThree, 4, 1));
  assert(1 == getBits(sixtyThree, 5, 5));
  assert(3 == getBits(sixtyThree, 7, 4));

  assert(max == getBits(max, 31, 0));
  assert(2047 == getBits(max, 31, 21));
  assert(3 == getBits(max, 2, 1));
  assert(31 == getBits(max, 6, 2));


  // Get flag
  assert(!getFlag(zero, 0));
  assert(!getFlag(zero, 30));

  assert(getFlag(five, 0));
  assert(!getFlag(five, 1));
  assert(getFlag(five, 2));

  assert(getFlag(sixtyThree, 0));
  assert(getFlag(sixtyThree, 5));
  assert(!getFlag(sixtyThree, 6));

  assert(getFlag(max, 0));
  assert(getFlag(max, 8));


  // Get byte
  assert(zero == getByte(zero, 7));
  assert(zero == getByte(zero, 14));
  assert(zero == getByte(zero, 31));

  assert(five == getByte(five, 7));
  assert(2 == getByte(five, 8));
  assert(zero == getByte(five, 31));

  assert(sixtyThree == getByte(sixtyThree, 7));
  assert(31 == getByte(sixtyThree, 8));
  assert(zero == getByte(sixtyThree, 31));

  assert(UINT8_MAX == getByte(max, 7));
  assert(UINT8_MAX == getByte(max, 8));
  assert(UINT8_MAX == getByte(max, 19));


  // Logical shift left
  assert(zero == lShiftLeft(zero, 0));
  assert(zero == lShiftLeft(zero, 4));
  assert(zero == lShiftLeft(zero, 72));

  assert(five == lShiftLeft(five, 0));
  assert(10 == lShiftLeft(five, 1));
  assert(40 == lShiftLeft(five, 3));

  assert(sixtyThree == lShiftLeft(sixtyThree, 0));
  assert(126 == lShiftLeft(sixtyThree, 1));
  assert(252 == lShiftLeft(sixtyThree, 2));
  assert(504 == lShiftLeft(sixtyThree, 3));

  assert(max == lShiftLeft(max, 0));
  assert(max - 1 == lShiftLeft(max, 1));
  assert(max - 3 == lShiftLeft(max, 2));
  assert(max - 7 == lShiftLeft(max, 3));


  // Logical shift right
  assert(zero == lShiftRight(zero, 0));
  assert(zero == lShiftRight(zero, 4));
  assert(zero == lShiftRight(zero, 72));

  assert(five == lShiftRight(five, 0));
  assert(2 == lShiftRight(five, 1));
  assert(0 == lShiftRight(five, 3));

  assert(sixtyThree == lShiftRight(sixtyThree, 0));
  assert(31 == lShiftRight(sixtyThree, 1));
  assert(15 == lShiftRight(sixtyThree, 2));
  assert(7 == lShiftRight(sixtyThree, 3));

  assert(max == lShiftRight(max, 0));
  assert(max - maxMSb == lShiftRight(max, 1));
  assert(max - maxMSb - max2MSb == lShiftRight(max, 2));
  assert(max - maxMSb - max2MSb - max3MSb == lShiftRight(max, 3));


  // Arithmetic shift right
  assert(zero == aShiftRight(zero, 0));
  assert(zero == aShiftRight(zero, 4));
  assert(zero == aShiftRight(zero, 72));

  assert(five == aShiftRight(five, 0));
  assert(2 == aShiftRight(five, 1));
  assert(0 == aShiftRight(five, 3));

  assert(sixtyThree == aShiftRight(sixtyThree, 0));
  assert(31 == aShiftRight(sixtyThree, 1));
  assert(15 == aShiftRight(sixtyThree, 2));
  assert(7 == aShiftRight(sixtyThree, 3));

  assert(max == aShiftRight(max, 0));
  assert(max == aShiftRight(max, 1));
  assert(max == aShiftRight(max, 22));
  assert(max - 1 == aShiftRight(max - 2, 1));
  assert(max == aShiftRight(max - 2, 2));


  // Rotate right
  assert(zero == rotateRight(zero, 0));
  assert(zero == rotateRight(zero, 4));
  assert(zero == rotateRight(zero, 72));

  assert(five == rotateRight(five, 0));
  assert(2147483650 == rotateRight(five, 1));
  assert(2684354560 == rotateRight(five, 3));

  assert(sixtyThree == rotateRight(sixtyThree, 0));
  assert(2147483679 == rotateRight(sixtyThree, 1));
  assert(3221225487 == rotateRight(sixtyThree, 2));
  assert(3758096391 == rotateRight(sixtyThree, 3));

  assert(max == rotateRight(max, 0));
  assert(max == rotateRight(max, 1));
  assert(max == rotateRight(max, 3));


  // Left pad zeros
  assert(0 == leftPadZeros(0));
  assert(242 == leftPadZeros(242));
  assert(1 == leftPadZeros(1));
  assert(UINT8_MAX == leftPadZeros(UINT8_MAX));


  return 0;
}
