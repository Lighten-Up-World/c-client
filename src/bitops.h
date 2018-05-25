//
// Created by Matt Malarkey on 24/05/2018.
//

#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

word_t getBits(word_t inst, byte_t x, byte_t y);

word_t lShiftLeft(word_t value, byte_t shift);

word_t lShiftRight(word_t value, byte_t shift);

word_t aShiftRight(word_t value, byte_t shift);

word_t rotateRight(word_t value, byte_t shift);

word_t leftPadZeros(byte_t value);

#endif
