#include "unity.h"
#include "../utils/bitops.h"

word_t minus42 = -42;
word_t minus1 = -1;
word_t zero = 0;
word_t five = 5;
word_t sixtyThree = 63;
word_t max = UINT32_MAX;
word_t maxMSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 1);
word_t max2MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 2);
word_t max3MSb = ((uint8_t) 0x1) << (sizeof(word_t) * 8 - 3);

void test_getBits(void) {

  TEST_ASSERT_EQUAL(zero, getBits(zero, 0, 0));
  TEST_ASSERT_EQUAL(zero, getBits(zero, 1, 0));
  TEST_ASSERT_EQUAL(zero, getBits(zero, 17, 6));

  TEST_ASSERT_EQUAL(zero, getBits(five, 31, 21));
  TEST_ASSERT_EQUAL(2, getBits(five, 2, 1));
  TEST_ASSERT_EQUAL(1, getBits(five, 2, 2));

  TEST_ASSERT_EQUAL(zero, getBits(sixtyThree, 31, 29));
  TEST_ASSERT_EQUAL(15, getBits(sixtyThree, 4, 1));
  TEST_ASSERT_EQUAL(1, getBits(sixtyThree, 5, 5));
  TEST_ASSERT_EQUAL(3, getBits(sixtyThree, 7, 4));

  TEST_ASSERT_EQUAL(2047, getBits(max, 31, 21));
  TEST_ASSERT_EQUAL(3, getBits(max, 2, 1));
  TEST_ASSERT_EQUAL(31, getBits(max, 6, 2));
}

void test_getFlag(void) {
  // Get flag
  TEST_ASSERT_FALSE(getFlag(zero, 0));
  TEST_ASSERT_FALSE(getFlag(zero, 30));

  TEST_ASSERT_TRUE(getFlag(five, 0));
  TEST_ASSERT_FALSE(getFlag(five, 1));
  TEST_ASSERT_TRUE(getFlag(five, 2));

  TEST_ASSERT_TRUE(getFlag(sixtyThree, 0));
  TEST_ASSERT_TRUE(getFlag(sixtyThree, 5));
  TEST_ASSERT_FALSE(getFlag(sixtyThree, 6));

  TEST_ASSERT_TRUE(getFlag(max, 0));
  TEST_ASSERT_TRUE(getFlag(max, 8));
}

void test_getByte(void){
  // Get byte
  TEST_ASSERT_EQUAL(zero, getByte(zero, 7));
  TEST_ASSERT_EQUAL(zero, getByte(zero, 14));
  TEST_ASSERT_EQUAL(zero, getByte(zero, 31));

  TEST_ASSERT_EQUAL(five, getByte(five, 7));
  TEST_ASSERT_EQUAL(2, getByte(five, 8));
  TEST_ASSERT_EQUAL(zero, getByte(five, 31));

  TEST_ASSERT_EQUAL(sixtyThree, getByte(sixtyThree, 7));
  TEST_ASSERT_EQUAL(31, getByte(sixtyThree, 8));
  TEST_ASSERT_EQUAL(zero, getByte(sixtyThree, 31));

  TEST_ASSERT_EQUAL(UINT8_MAX, getByte(max, 7));
  TEST_ASSERT_EQUAL(UINT8_MAX, getByte(max, 8));
  TEST_ASSERT_EQUAL(UINT8_MAX, getByte(max, 19));
}

void test_lShift(void){
  // Logical shift left
  TEST_ASSERT_EQUAL(zero, lShiftLeft(zero, 0));
  TEST_ASSERT_EQUAL(zero, lShiftLeft(zero, 4));
  TEST_ASSERT_EQUAL(zero, lShiftLeft(zero, 72));

  TEST_ASSERT_EQUAL(five, lShiftLeft(five, 0));
  TEST_ASSERT_EQUAL(10, lShiftLeft(five, 1));
  TEST_ASSERT_EQUAL(40, lShiftLeft(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, lShiftLeft(sixtyThree, 0));
  TEST_ASSERT_EQUAL(126, lShiftLeft(sixtyThree, 1));
  TEST_ASSERT_EQUAL(252, lShiftLeft(sixtyThree, 2));
  TEST_ASSERT_EQUAL(504, lShiftLeft(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, lShiftLeft(max, 0));
  TEST_ASSERT_EQUAL(max - 1, lShiftLeft(max, 1));
  TEST_ASSERT_EQUAL(max - 3, lShiftLeft(max, 2));
  TEST_ASSERT_EQUAL(max - 7, lShiftLeft(max, 3));


  // Logical shift right
  TEST_ASSERT_EQUAL(zero, lShiftRight(zero, 0));
  TEST_ASSERT_EQUAL(zero, lShiftRight(zero, 4));
  TEST_ASSERT_EQUAL(zero, lShiftRight(zero, 72));

  TEST_ASSERT_EQUAL(five, lShiftRight(five, 0));
  TEST_ASSERT_EQUAL(2, lShiftRight(five, 1));
  TEST_ASSERT_EQUAL(0, lShiftRight(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, lShiftRight(sixtyThree, 0));
  TEST_ASSERT_EQUAL(31, lShiftRight(sixtyThree, 1));
  TEST_ASSERT_EQUAL(15, lShiftRight(sixtyThree, 2));
  TEST_ASSERT_EQUAL(7, lShiftRight(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, lShiftRight(max, 0));
  TEST_ASSERT_EQUAL(max - maxMSb, lShiftRight(max, 1));
  TEST_ASSERT_EQUAL(max - maxMSb - max2MSb, lShiftRight(max, 2));
  TEST_ASSERT_EQUAL(max - maxMSb - max2MSb - max3MSb, lShiftRight(max, 3));
}

void test_aShift(void){


    // Arithmetic shift right
    TEST_ASSERT_EQUAL(zero, aShiftRight(zero, 0));
    TEST_ASSERT_EQUAL(zero, aShiftRight(zero, 4));
    TEST_ASSERT_EQUAL(zero, aShiftRight(zero, 72));

    TEST_ASSERT_EQUAL(five, aShiftRight(five, 0));
    TEST_ASSERT_EQUAL(2, aShiftRight(five, 1));
    TEST_ASSERT_EQUAL(0, aShiftRight(five, 3));

    TEST_ASSERT_EQUAL(sixtyThree, aShiftRight(sixtyThree, 0));
    TEST_ASSERT_EQUAL(31, aShiftRight(sixtyThree, 1));
    TEST_ASSERT_EQUAL(15, aShiftRight(sixtyThree, 2));
    TEST_ASSERT_EQUAL(7, aShiftRight(sixtyThree, 3));

    TEST_ASSERT_EQUAL(max, aShiftRight(max, 0));
    TEST_ASSERT_EQUAL(max, aShiftRight(max, 1));
    TEST_ASSERT_EQUAL(max, aShiftRight(max, 22));
    TEST_ASSERT_EQUAL(max - 1, aShiftRight(max - 2, 1));
    TEST_ASSERT_EQUAL(max, aShiftRight(max - 2, 2));
}

void test_rotate(void){


    // Rotate right
    TEST_ASSERT_EQUAL(zero, rotateRight(zero, 0));
    TEST_ASSERT_EQUAL(zero, rotateRight(zero, 4));
    TEST_ASSERT_EQUAL(zero, rotateRight(zero, 72));

    TEST_ASSERT_EQUAL(five, rotateRight(five, 0));
    TEST_ASSERT_EQUAL(2147483650, rotateRight(five, 1));
    TEST_ASSERT_EQUAL(2684354560, rotateRight(five, 3));

    TEST_ASSERT_EQUAL(sixtyThree, rotateRight(sixtyThree, 0));
    TEST_ASSERT_EQUAL(2147483679, rotateRight(sixtyThree, 1));
    TEST_ASSERT_EQUAL(3221225487, rotateRight(sixtyThree, 2));
    TEST_ASSERT_EQUAL(3758096391, rotateRight(sixtyThree, 3));

    TEST_ASSERT_EQUAL(max, rotateRight(max, 0));
    TEST_ASSERT_EQUAL(max, rotateRight(max, 1));
    TEST_ASSERT_EQUAL(max, rotateRight(max, 3));
}

void test_leftPadZeros(void){
    // Left pad zeros
    TEST_ASSERT_EQUAL(0, leftPadZeros(0));
    TEST_ASSERT_EQUAL(242, leftPadZeros(242));
    TEST_ASSERT_EQUAL(1, leftPadZeros(1));
    TEST_ASSERT_EQUAL(UINT8_MAX, leftPadZeros(UINT8_MAX));
}

void test_carry(void){

}

void test_negate(void){
  TEST_ASSERT_EQUAL(42, negate(minus42));
  TEST_ASSERT_EQUAL(1, negate(minus1));
  TEST_ASSERT_EQUAL((word_t) -63, negate(sixtyThree));
}

void test_isNegative(void){
  TEST_ASSERT_TRUE(isNegative(minus1));
  TEST_ASSERT_TRUE(isNegative(minus42));
  TEST_ASSERT_FALSE(isNegative(zero));
  TEST_ASSERT_FALSE(isNegative(sixtyThree));
  TEST_ASSERT_TRUE(isNegative(UINT32_MAX));
}
