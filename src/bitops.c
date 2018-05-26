#include <stdlib.h>
#include <assert.h>
#include <printf.h>
#include "bitops.h"

/*
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *
 *  @param inst: the instruction to get bits from
 *  @param x: the MSb of interval to return
 *  @param y: the LSb of interval to return
 *  @return
 */
word_t getBits(word_t inst, byte_t x, byte_t y) {

  //shift down so that x is base

  //mask so that everything after y is zero




  //TODO: assertions
  //TODO: check this works with endian-nes of inst. given
  //return rShiftRight(inst, y);
  return 0;
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
  return value << shift;
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
  return value >> shift;
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
  word_t msb = value >> (sizeof(word_t) * 8 - 1);

  if (msb == 0) {
    return value >> shift;
  }

  word_t msbOnly = msb << ((sizeof(word_t) * 8) - 1);
  for (int i = 0; i < shift; i++) {
    value = value >> ((uint8_t) 1);
    value = value | msbOnly;
  }
  return value;
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
  word_t lsb;
  for (int i = 0; i < rotate; i++) {
    lsb = value & ((uint8_t) 0x1);
    value = value >> ((uint8_t) 1);
    value = value | (lsb << ((sizeof(word_t) * 8) - 1));
  }
  return value;
}

/*
 *  Pad out a byte value to a word value, with zeros
 *
 *    @param value: the value to zero extend
 *    @return the zero extended value
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

