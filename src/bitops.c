#include <stdlib.h>
#include <assert.h>
#include <printf.h>
#include "bitops.h"

const uint8_t U_ONE = 1;

//TODO: test, docs, add to header
//change bool to flag_t after testing
bool getFlag(word_t inst, byte_t pos) {
  return (bool) getBits(inst, pos, pos);
}

//TODO: test, docs, header, check if -8 is the right idea
byte_t getByte(word_t inst, byte_t pos) {
  return (byte_t) getBits(inst, pos, (byte_t) (pos - 8));
}

/*
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *  Limits are inclusive.
 *
 *  @param inst: the instruction to get bits from
 *  @param x: the MSb of interval to return
 *  @param y: the LSb of interval to return
 *  @return
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
    value = value >> U_ONE;
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
    lsb = value & U_ONE;
    value = value >> U_ONE;
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
