//
// Created by Matt Malarkey on 24/05/2018.
//

#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

flag_t getFlag(word_t inst, byte_t pos);

byte_t getByte(word_t inst, byte_t pos);

byte_t getNibble(word_t inst, byte_t pos);

word_t getBits(word_t inst, byte_t x, byte_t y);

word_t lShiftLeft(word_t value, byte_t shift);

word_t lShiftRight(word_t value, byte_t shift);

word_t aShiftRight(word_t value, byte_t shift);

word_t rotateRight(word_t value, byte_t shift);

shift_result_t lShiftLeftC(word_t value, byte_t shift);

shift_result_t lShiftRightC(word_t value, byte_t shift);

shift_result_t aShiftRightC(word_t value, byte_t shift);

shift_result_t rotateRightC(word_t value, byte_t shift);

flag_t isNegative(word_t word);
word_t negate(word_t word)

word_t leftPadZeros(byte_t value);

#endif
