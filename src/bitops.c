// TODO: Extract bits x to y from 32 bit instruction
// TODO: Make 32 bit integer from 8 bit for shifting

#include <stdlib.h>
#include <assert.h>
#include "bitops.h"

// Main for testing
int main(int argc, char **argv) {

  word_t zero = 0;
  word_t num = 5;

  assert(zero == logicalShiftLeft(zero, 0));
  assert(zero == logicalShiftLeft(zero, 4));
  assert(zero == logicalShiftLeft(zero, 72));

  assert(num == logicalShiftLeft(num, 0));
  assert(10 == logicalShiftLeft(num, 1));
  assert(40 == logicalShiftLeft(num, 3));

  return 0;
}


//zero fill from right, discard carry
word_t logicalShiftLeft(word_t value, byte_t shift) {
  return value << shift;
}

word_t logicalShiftRight(word_t value, byte_t shift);

word_t arithShiftRight(word_t value, byte_t shift);

word_t rotateRight(word_t value, byte_t shift);
