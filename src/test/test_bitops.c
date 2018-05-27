#include "unity.h"
#include "../bitops.h"

word_t zero = 0;
word_t five = 5;
word_t sixtyThree = 63;
word_t max = UINT32_MAX;
word_t maxMSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 1);
word_t max2MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 2);
word_t max3MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 3);

void test_getBits(void) {

  TEST_ASSERT(zero == getBits(zero, 0, 0));
  TEST_ASSERT(zero == getBits(zero, 1, 0));
  TEST_ASSERT(zero == getBits(zero, 17, 6));

  TEST_ASSERT(five == getBits(five, 31, 0));
  TEST_ASSERT(zero == getBits(five, 31, 21));
  TEST_ASSERT(2 == getBits(five, 2, 1));
  TEST_ASSERT(1 == getBits(five, 2, 2));

  TEST_ASSERT(sixtyThree == getBits(sixtyThree, 31, 0));
  TEST_ASSERT(zero == getBits(sixtyThree, 31, 29));
  TEST_ASSERT(15 == getBits(sixtyThree, 4, 1));
  TEST_ASSERT(1 == getBits(sixtyThree, 5, 5));
  TEST_ASSERT(3 == getBits(sixtyThree, 7, 4));

  TEST_ASSERT(max == getBits(max, 31, 0));
  TEST_ASSERT(2047 == getBits(max, 31, 21));
  TEST_ASSERT(3 == getBits(max, 2, 1));
  TEST_ASSERT(31 == getBits(max, 6, 2));
}

void test_getFlag(void) {
  // Get flag
  TEST_ASSERT(!getFlag(zero, 0));
  TEST_ASSERT(!getFlag(zero, 30));

  TEST_ASSERT(getFlag(five, 0));
  TEST_ASSERT(!getFlag(five, 1));
  TEST_ASSERT(getFlag(five, 2));

  TEST_ASSERT(getFlag(sixtyThree, 0));
  TEST_ASSERT(getFlag(sixtyThree, 5));
  TEST_ASSERT(!getFlag(sixtyThree, 6));

  TEST_ASSERT(getFlag(max, 0));
  TEST_ASSERT(getFlag(max, 8));
}

void test_getByte(void){
  // Get byte
  TEST_ASSERT(zero == getByte(zero, 7));
  TEST_ASSERT(zero == getByte(zero, 14));
  TEST_ASSERT(zero == getByte(zero, 31));

  TEST_ASSERT(five == getByte(five, 7));
  TEST_ASSERT(2 == getByte(five, 8));
  TEST_ASSERT(zero == getByte(five, 31));

  TEST_ASSERT(sixtyThree == getByte(sixtyThree, 7));
  TEST_ASSERT(31 == getByte(sixtyThree, 8));
  TEST_ASSERT(zero == getByte(sixtyThree, 31));

  TEST_ASSERT(UINT8_MAX == getByte(max, 7));
  TEST_ASSERT(UINT8_MAX == getByte(max, 8));
  TEST_ASSERT(UINT8_MAX == getByte(max, 19));
}

void test_lShift(void){
  // Logical shift left
  TEST_ASSERT(zero == lShiftLeft(zero, 0));
  TEST_ASSERT(zero == lShiftLeft(zero, 4));
  TEST_ASSERT(zero == lShiftLeft(zero, 72));

  TEST_ASSERT(five == lShiftLeft(five, 0));
  TEST_ASSERT(10 == lShiftLeft(five, 1));
  TEST_ASSERT(40 == lShiftLeft(five, 3));

  TEST_ASSERT(sixtyThree == lShiftLeft(sixtyThree, 0));
  TEST_ASSERT(126 == lShiftLeft(sixtyThree, 1));
  TEST_ASSERT(252 == lShiftLeft(sixtyThree, 2));
  TEST_ASSERT(504 == lShiftLeft(sixtyThree, 3));

  TEST_ASSERT(max == lShiftLeft(max, 0));
  TEST_ASSERT(max - 1 == lShiftLeft(max, 1));
  TEST_ASSERT(max - 3 == lShiftLeft(max, 2));
  TEST_ASSERT(max - 7 == lShiftLeft(max, 3));


  // Logical shift right
  TEST_ASSERT(zero == lShiftRight(zero, 0));
  TEST_ASSERT(zero == lShiftRight(zero, 4));
  TEST_ASSERT(zero == lShiftRight(zero, 72));

  TEST_ASSERT(five == lShiftRight(five, 0));
  TEST_ASSERT(2 == lShiftRight(five, 1));
  TEST_ASSERT(0 == lShiftRight(five, 3));

  TEST_ASSERT(sixtyThree == lShiftRight(sixtyThree, 0));
  TEST_ASSERT(31 == lShiftRight(sixtyThree, 1));
  TEST_ASSERT(15 == lShiftRight(sixtyThree, 2));
  TEST_ASSERT(7 == lShiftRight(sixtyThree, 3));

  TEST_ASSERT(max == lShiftRight(max, 0));
  TEST_ASSERT(max - maxMSb == lShiftRight(max, 1));
  TEST_ASSERT(max - maxMSb - max2MSb == lShiftRight(max, 2));
  TEST_ASSERT(max - maxMSb - max2MSb - max3MSb == lShiftRight(max, 3));
}
void test_aShift(void){


    // Arithmetic shift right
    TEST_ASSERT(zero == aShiftRight(zero, 0));
    TEST_ASSERT(zero == aShiftRight(zero, 4));
    TEST_ASSERT(zero == aShiftRight(zero, 72));

    TEST_ASSERT(five == aShiftRight(five, 0));
    TEST_ASSERT(2 == aShiftRight(five, 1));
    TEST_ASSERT(0 == aShiftRight(five, 3));

    TEST_ASSERT(sixtyThree == aShiftRight(sixtyThree, 0));
    TEST_ASSERT(31 == aShiftRight(sixtyThree, 1));
    TEST_ASSERT(15 == aShiftRight(sixtyThree, 2));
    TEST_ASSERT(7 == aShiftRight(sixtyThree, 3));

    TEST_ASSERT(max == aShiftRight(max, 0));
    TEST_ASSERT(max == aShiftRight(max, 1));
    TEST_ASSERT(max == aShiftRight(max, 22));
    TEST_ASSERT(max - 1 == aShiftRight(max - 2, 1));
    TEST_ASSERT(max == aShiftRight(max - 2, 2));
}

void test_rotate(void){


    // Rotate right
    TEST_ASSERT(zero == rotateRight(zero, 0));
    TEST_ASSERT(zero == rotateRight(zero, 4));
    TEST_ASSERT(zero == rotateRight(zero, 72));

    TEST_ASSERT(five == rotateRight(five, 0));
    TEST_ASSERT(2147483650 == rotateRight(five, 1));
    TEST_ASSERT(2684354560 == rotateRight(five, 3));

    TEST_ASSERT(sixtyThree == rotateRight(sixtyThree, 0));
    TEST_ASSERT(2147483679 == rotateRight(sixtyThree, 1));
    TEST_ASSERT(3221225487 == rotateRight(sixtyThree, 2));
    TEST_ASSERT(3758096391 == rotateRight(sixtyThree, 3));

    TEST_ASSERT(max == rotateRight(max, 0));
    TEST_ASSERT(max == rotateRight(max, 1));
    TEST_ASSERT(max == rotateRight(max, 3));
}

void test_leftPadZeros(void){
    // Left pad zeros
    TEST_ASSERT(0 == leftPadZeros(0));
    TEST_ASSERT(242 == leftPadZeros(242));
    TEST_ASSERT(1 == leftPadZeros(1));
    TEST_ASSERT(UINT8_MAX == leftPadZeros(UINT8_MAX));
}
