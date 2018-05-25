//
// Created by Matt Malarkey on 24/05/2018.
//

// TODO: Extract bits x to y from 32 bit instruction
// TODO: Make 32 bit integer from 8 bit for shifting

#ifndef ARM11_22_BITOPS_H
#define ARM11_22_BITOPS_H

#include "arm.h"

// Copy bits x to y from inst to ext
// Pre: x >= y
// Pre: 0 <= x,y <= length(instr)
// Pre: ext must be of size y - x + 1
void getBits(word_t *inst, int *ext, int x, int y);

// Get the shifted 32 bit integer from the full instruction
// Pre: instr must be DP format
val_t getDataOp2(word_t instr);

#endif ARM11_22_BITOPS_H
