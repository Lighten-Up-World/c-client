#include <test/unity/unity.h>
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
  TEST_ASSERT_EQUAL(zero, get_bits(zero, 0, 0));
  TEST_ASSERT_EQUAL(zero, get_bits(zero, 1, 0));
  TEST_ASSERT_EQUAL(zero, get_bits(zero, 17, 6));

  TEST_ASSERT_EQUAL(zero, get_bits(five, 31, 21));
  TEST_ASSERT_EQUAL(2, get_bits(five, 2, 1));
  TEST_ASSERT_EQUAL(1, get_bits(five, 2, 2));

  TEST_ASSERT_EQUAL(zero, get_bits(sixtyThree, 31, 29));
  TEST_ASSERT_EQUAL(15, get_bits(sixtyThree, 4, 1));
  TEST_ASSERT_EQUAL(1, get_bits(sixtyThree, 5, 5));
  TEST_ASSERT_EQUAL(3, get_bits(sixtyThree, 7, 4));

  TEST_ASSERT_EQUAL(2047, get_bits(max, 31, 21));
  TEST_ASSERT_EQUAL(3, get_bits(max, 2, 1));
  TEST_ASSERT_EQUAL(31, get_bits(max, 6, 2));
}

void test_getFlag(void) {
  TEST_ASSERT_FALSE(get_flag(zero, 0));
  TEST_ASSERT_FALSE(get_flag(zero, 30));

  TEST_ASSERT_TRUE(get_flag(five, 0));
  TEST_ASSERT_FALSE(get_flag(five, 1));
  TEST_ASSERT_TRUE(get_flag(five, 2));

  TEST_ASSERT_TRUE(get_flag(sixtyThree, 0));
  TEST_ASSERT_TRUE(get_flag(sixtyThree, 5));
  TEST_ASSERT_FALSE(get_flag(sixtyThree, 6));

  TEST_ASSERT_TRUE(get_flag(max, 0));
  TEST_ASSERT_TRUE(get_flag(max, 8));
}

void test_getByte(void){
  TEST_ASSERT_EQUAL(zero, get_byte(zero, 7));
  TEST_ASSERT_EQUAL(zero, get_byte(zero, 14));
  TEST_ASSERT_EQUAL(zero, get_byte(zero, 31));

  TEST_ASSERT_EQUAL(five, get_byte(five, 7));
  TEST_ASSERT_EQUAL(2, get_byte(five, 8));
  TEST_ASSERT_EQUAL(zero, get_byte(five, 31));

  TEST_ASSERT_EQUAL(sixtyThree, get_byte(sixtyThree, 7));
  TEST_ASSERT_EQUAL(31, get_byte(sixtyThree, 8));
  TEST_ASSERT_EQUAL(zero, get_byte(sixtyThree, 31));

  TEST_ASSERT_EQUAL(UINT8_MAX, get_byte(max, 7));
  TEST_ASSERT_EQUAL(UINT8_MAX, get_byte(max, 8));
  TEST_ASSERT_EQUAL(UINT8_MAX, get_byte(max, 19));
}

void test_lShift(void){
  // Logical shift left
  TEST_ASSERT_EQUAL(zero, l_shift_left(zero, 0));
  TEST_ASSERT_EQUAL(zero, l_shift_left(zero, 4));
  TEST_ASSERT_EQUAL(zero, l_shift_left(zero, 72));

  TEST_ASSERT_EQUAL(five, l_shift_left(five, 0));
  TEST_ASSERT_EQUAL(10, l_shift_left(five, 1));
  TEST_ASSERT_EQUAL(40, l_shift_left(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, l_shift_left(sixtyThree, 0));
  TEST_ASSERT_EQUAL(126, l_shift_left(sixtyThree, 1));
  TEST_ASSERT_EQUAL(252, l_shift_left(sixtyThree, 2));
  TEST_ASSERT_EQUAL(504, l_shift_left(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, l_shift_left(max, 0));
  TEST_ASSERT_EQUAL(max - 1, l_shift_left(max, 1));
  TEST_ASSERT_EQUAL(max - 3, l_shift_left(max, 2));
  TEST_ASSERT_EQUAL(max - 7, l_shift_left(max, 3));


  // Logical shift right
  TEST_ASSERT_EQUAL(zero, l_shift_right(zero, 0));
  TEST_ASSERT_EQUAL(zero, l_shift_right(zero, 4));
  TEST_ASSERT_EQUAL(zero, l_shift_right(zero, 72));

  TEST_ASSERT_EQUAL(five, l_shift_right(five, 0));
  TEST_ASSERT_EQUAL(2, l_shift_right(five, 1));
  TEST_ASSERT_EQUAL(0, l_shift_right(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, l_shift_right(sixtyThree, 0));
  TEST_ASSERT_EQUAL(31, l_shift_right(sixtyThree, 1));
  TEST_ASSERT_EQUAL(15, l_shift_right(sixtyThree, 2));
  TEST_ASSERT_EQUAL(7, l_shift_right(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, l_shift_right(max, 0));
  TEST_ASSERT_EQUAL(max - maxMSb, l_shift_right(max, 1));
  TEST_ASSERT_EQUAL(max - maxMSb - max2MSb, l_shift_right(max, 2));
  TEST_ASSERT_EQUAL(max - maxMSb - max2MSb - max3MSb, l_shift_right(max, 3));
}

void test_aShift(void) {
  TEST_ASSERT_EQUAL(zero, a_shift_right(zero, 0));
  TEST_ASSERT_EQUAL(zero, a_shift_right(zero, 4));
  TEST_ASSERT_EQUAL(zero, a_shift_right(zero, 72));

  TEST_ASSERT_EQUAL(five, a_shift_right(five, 0));
  TEST_ASSERT_EQUAL(2, a_shift_right(five, 1));
  TEST_ASSERT_EQUAL(0, a_shift_right(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, a_shift_right(sixtyThree, 0));
  TEST_ASSERT_EQUAL(31, a_shift_right(sixtyThree, 1));
  TEST_ASSERT_EQUAL(15, a_shift_right(sixtyThree, 2));
  TEST_ASSERT_EQUAL(7, a_shift_right(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, a_shift_right(max, 0));
  TEST_ASSERT_EQUAL(max, a_shift_right(max, 1));
  TEST_ASSERT_EQUAL(max, a_shift_right(max, 22));
  TEST_ASSERT_EQUAL(max - 1, a_shift_right(max - 2, 1));
  TEST_ASSERT_EQUAL(max, a_shift_right(max - 2, 2));
}

void test_rotate(void) {
  // Rotate right
  TEST_ASSERT_EQUAL(zero, rotate_right(zero, 0));
  TEST_ASSERT_EQUAL(zero, rotate_right(zero, 4));
  TEST_ASSERT_EQUAL(zero, rotate_right(zero, 72));

  TEST_ASSERT_EQUAL(five, rotate_right(five, 0));
  TEST_ASSERT_EQUAL(2147483650, rotate_right(five, 1));
  TEST_ASSERT_EQUAL(2684354560, rotate_right(five, 3));

  TEST_ASSERT_EQUAL(sixtyThree, rotate_right(sixtyThree, 0));
  TEST_ASSERT_EQUAL(2147483679, rotate_right(sixtyThree, 1));
  TEST_ASSERT_EQUAL(3221225487, rotate_right(sixtyThree, 2));
  TEST_ASSERT_EQUAL(3758096391, rotate_right(sixtyThree, 3));

  TEST_ASSERT_EQUAL(max, rotate_right(max, 0));
  TEST_ASSERT_EQUAL(max, rotate_right(max, 1));
  TEST_ASSERT_EQUAL(max, rotate_right(max, 3));

  // Rotate left
  TEST_ASSERT_EQUAL(zero, rotate_left(zero, 0));
  TEST_ASSERT_EQUAL(zero, rotate_left(zero, 4));
  TEST_ASSERT_EQUAL(zero, rotate_left(zero, 72));

  TEST_ASSERT_EQUAL(five, rotate_left(five, 0));
  TEST_ASSERT_EQUAL(five, rotate_left(2147483650, 1));
  TEST_ASSERT_EQUAL(five, rotate_left(2684354560, 3));

  TEST_ASSERT_EQUAL(sixtyThree, rotate_left(sixtyThree, 0));
  TEST_ASSERT_EQUAL(sixtyThree, rotate_left(2147483679, 1));
  TEST_ASSERT_EQUAL(sixtyThree, rotate_left(3221225487, 2));
  TEST_ASSERT_EQUAL(sixtyThree, rotate_left(3758096391, 3));

  TEST_ASSERT_EQUAL(max, rotate_left(max, 0));
  TEST_ASSERT_EQUAL(max, rotate_left(max, 1));
  TEST_ASSERT_EQUAL(max, rotate_left(max, 3));
}

void test_leftPadZeros(void){
  TEST_ASSERT_EQUAL(0, left_pad_zeros(0));
  TEST_ASSERT_EQUAL(242, left_pad_zeros(242));
  TEST_ASSERT_EQUAL(1, left_pad_zeros(1));
  TEST_ASSERT_EQUAL(UINT8_MAX, left_pad_zeros(UINT8_MAX));
}

void test_carry(void){

}

void test_negate(void){
  TEST_ASSERT_EQUAL(42, negate(minus42));
  TEST_ASSERT_EQUAL(1, negate(minus1));
  TEST_ASSERT_EQUAL((word_t) -63, negate(sixtyThree));
}

void test_isNegative(void){
  TEST_ASSERT_TRUE(is_negative(minus1));
  TEST_ASSERT_TRUE(is_negative(minus42));
  TEST_ASSERT_FALSE(is_negative(zero));
  TEST_ASSERT_FALSE(is_negative(sixtyThree));
  TEST_ASSERT_TRUE(is_negative(UINT32_MAX));
}


