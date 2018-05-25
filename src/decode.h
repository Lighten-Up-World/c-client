#ifndef ARM11_22_DECODE_H
#define ARM11_22_DECODE_H

#include "arm.h"
#include "instructions.h"

#define COND_START 31
#define COND_END 28

instruction_t decodeWord(word_t word);

#endif