//
// Created by Matt Malarkey on 24/05/2018.
//

// TODO: Extract bits x to y from 32 bit instruction
// TODO: Make 32 bit integer from 8 bit for shifting (0 pad)
// TODO: Add all shifting operations

#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

word_t getBits(word_t inst, int x, int y);

word_t lShiftLeft(word_t value, byte_t shift);

word_t lShiftRight(word_t value, byte_t shift);

word_t aShiftRight(word_t value, byte_t shift);

word_t rotateRight(word_t value, byte_t shift);

word_t leftPadZeros(byte_t);

#endif
