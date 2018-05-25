#include "arm.h"
#include "instructions.h"
#include <assert.h>
#include <stdio.h>

//// DP ////

/*
 * @params -
 *
 */

void decodeDp(instruction_t* instructionPtr, word_t word);

//// MUL ////


//// SDT ////


//// BRN ////


//// HAL ////


//// INSTRUCTION TYPE ////


//// DECODE ENTRY ////

/*
 * @params - word_t word is the binary instruction to decode
 * @return - instruction_t* is a pointer to the decoded instruction
 */

instruction_t* decodeWord(word_t word) {
    assert(word != NULL);

    instruction_t instruction;
    instruction_t* instructionPtr = &instruction;

    //Call functions to decode instruction

    return instructionPtr;
}
