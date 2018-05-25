// TODO: Extract bits x to y from 32 bit instruction
// TODO: Make 32 bit integer from 8 bit for shifting

#include <stdlib.h>
#include <assert.h>
#include "bitops.h"

// Main for testing
int main(int argc, char **argv) {

  word_t zero = 0;
  word_t five = 5;
  word_t sixtyThree = 63;

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

/*
 *  Arithmetic shift right
 *
 *  @param value: The value to shift
 *  @param shift: The amount to shift by
 *  @return the shifted value
 */
word_t aShiftRight(word_t value, byte_t shift) {
  return 0;
}

/*
 *  Rotate right
 *
 *  @param value: The value to shift
 *  @param rotate: The amount to rotate by
 *  @return the shifted value
 */
word_t rotateRight(word_t value, byte_t rotate) {
  return 0;
}
