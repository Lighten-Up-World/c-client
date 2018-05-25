#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include <assert.h>
#include <stdio.h>

/*
 * @params - dp_instruction_t* dp is a pointer to the data
 * processing instruction to decode. The method will need to determine
 * whether operand2 is an immediate value or not.
 * @return - void, the method will modify the value of dp in memory using
 * it's pointer.
 *
 */
void decodeOperand2(dp_instruction_t* dp);

//// DP ////

/*
 * @params - instruction_t* instructionPtr is a pointer to the instruction to
 * decode
 * @params - word_t word word_t word is the binary instruction to decode.
 * @return - void, we modify the value of the instruction using the pointer.
 */

void decodeDp(instruction_t* instructionPtr, word_t word) {
  assert(instructionPtr != NULL);
  assert(word != NULL);
  dp_instruction_t dp; // create dp instruction
  instructionPtr->i = dp; // set this to instruction
  dp_instruction_t* dpPtr = &dp; // create pointer to dp instruction


  dpPtr->cond = (byte_t) getBits(word, 31, 28);
  dpPtr->padding = 00;
  dpPtr->I = getBits(word, 25, 25);
  dpPtr->opcode = (byte_t) getBits(word, 24, 21);
  dpPtr->S = getBits(word, 20,20);
  dpPtr->rn = (byte_t) getBits(word, 19, 16);
  dpPtr->rd = (byte_t) getBits(word, 15, 12);

  decodeOperand2(dpPtr);
}

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
