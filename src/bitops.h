//
// Created by Matt Malarkey on 24/05/2018.
//

// TODO: Extract bits x to y from 32 bit instruction
// TODO: Make 32 bit integer from 8 bit for shifting (0 pad)
// TODO: Add all shifting operations

#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

// Copy bits x to y from inst to ext
// Pre: x >= y
// Pre: 0 <= x,y <= length(instr)
// Pre: ext must be of size y - x + 1
void getBits(word_t *inst, int *ext, int x, int y);

word_t bits(word_t *inst, int x, int y);

word_t logicalShiftLeft(word_t value, byte_t shift);

word_t logicalShiftRight(word_t value, byte_t shift);

word_t arithShiftRight(word_t value, byte_t shift);

word_t rotateRight(word_t value, byte_t shift);

// see Op2 on page7
// left pad with 0s
word_t leftPadZeros(byte_t);

#endif
