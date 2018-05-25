#include <stdlib.h>
#include <assert.h>
#include <printf.h>
#include "bitops.h"

// Main for testing
int main(int argc, char **argv) {

  word_t zero = 0;
  word_t five = 5;
  word_t sixtyThree = 63;
  word_t max = UINT32_MAX;

  // Logical shift left
  assert(zero == lShiftLeft(zero, 0));
  assert(zero == lShiftLeft(zero, 4));
  assert(zero == lShiftLeft(zero, 72));
  assert(five == lShiftLeft(five, 0));
  assert(10 == lShiftLeft(five, 1));
  assert(40 == lShiftLeft(five, 3));

  // Logical shift right
  assert(zero == lShiftRight(zero, 0));
  assert(zero == lShiftRight(zero, 4));
  assert(zero == lShiftRight(zero, 72));
  assert(five == lShiftRight(five, 0));
  assert(2 == lShiftRight(five, 1));
  assert(0 == lShiftRight(five, 3));
  assert(31 == lShiftRight(sixtyThree, 1));
  assert(15 == lShiftRight(sixtyThree, 2));
  assert(7 == lShiftRight(sixtyThree, 3));

  //TODO
  // Arithmetic shift right
  assert(zero == aShiftRight(zero, 0));
  assert(zero == aShiftRight(zero, 4));
  assert(zero == aShiftRight(zero, 72));
  assert(five == aShiftRight(five, 0));
  assert(2 == aShiftRight(five, 1));
  assert(0 == aShiftRight(five, 3));
  assert(max == aShiftRight(max, 1));
  assert(max == aShiftRight(max, 22));
  assert(max - 1 == aShiftRight(max - 2, 1));
  assert(max == aShiftRight(max - 2, 2));

  //TODO
  // Rotate right
  assert(zero == rotateRight(zero, 0));
  assert(zero == rotateRight(zero, 4));
  assert(zero == rotateRight(zero, 72));
  assert(max == rotateRight(max, 0));
  assert(max == rotateRight(max, 1));
  assert(max == rotateRight(max, 3));

  //TODO
  // Left pad zeros
  assert(0 == leftPadZeros(0));
  assert(242 == leftPadZeros(242));
  assert(1 == leftPadZeros(1));

  byte_t x = 0;
  byte_t y = 0;
  word_t inst = 0;
  assert(true);

  return 0;
}

/*
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *  @param inst: the instruction to get bits from
 *  @param x: the MSb of interval to return
 *  @param y: the LSb of interval to return
 */
word_t getBits(word_t inst, byte_t x, byte_t y) {
  //TODO: check this works with endian-nes of inst. given
  //return rShiftRight(inst, y);
}

/*
 *  Logical shift left
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t lShiftLeft(word_t value, byte_t shift) {
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
  return value >> shift;
}

//TODO: test
/*
 *  Arithmetic shift right
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t aShiftRight(word_t value, byte_t shift) {
  word_t msb = (value >> (sizeof(word_t) * 8 - 1)) & ((uint8_t) 0x1);

  if (msb == 0) {
    // Since >> fills with zeros we can just return this
    return value >> shift;
  }

  for (int i = 0; i < shift; i++) {
    value = value >> ((uint8_t) 1);
    value = value | (msb << ((sizeof(word_t) * 8) - 1));
  }
  return value;
}

//TODO: test
/*
 *  Rotate right
 *
 *  @param value: The value to shift
 *  @param rotate: The amount to rotate by
 *  @return the shifted value
 */
word_t rotateRight(word_t value, byte_t rotate) {
  word_t lsb;
  for (int i = 0; i < rotate; i++) {
    lsb = value & ((uint8_t) 0x1);
    value = value >> ((uint8_t) 1);
    value = value | (lsb << ((sizeof(word_t) * 8) - 1));
  }
  return value;
}

//TODO: test
/*
 *  Pad out a byte value to a word value, with zeros
 *
 *    @param value: the value to zero extend
 *    @return the zero extended value
 */
word_t leftPadZeros(byte_t value) {
  return value;
}