#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include "decode.h"
#include <assert.h>
#include <stdio.h>

//// DP ////


//// MUL ////


//// SDT ////


//// BRN ////


//// HAL ////


//// INSTRUCTION TYPE ////

/*
 * Decode Instruction Type
 *
 * @param - instruction_t* instructionPtr is a pointer to the instruction_t
 * @param - word_t word is the binary instruction to decode
 * @return - void, all changes occur directly to instruction_t in memory.
 */

void decodeInstructionType(instruction_t* instructionPtr, word_t word){
    instruction_type_t instruction_type;
    if (word == 0x0){
        instruction_type = HAL;
    }else {
        word_t selectionBits = getBits(word, 27, 25);
        word_t pad9;
        switch (selectionBits) {
            case 0x0:
                pad9 = getBits(word, 7, 4);
                if (pad9 ^ 0x9) {
                    instruction_type = DP;
                } else {
                    instruction_type = MUL;
                }
                break;
            case 0x1:
                instruction_type = MUL;
                break;
            case 0x5:
                instruction_type = BRN;
                break;
            default:
                instruction_type = SDT;
                break;
        }
    }
    instructionPtr->type = instruction_type;
}


//// DECODE ENTRY ////

/*
 * Decode Word (entry point to file)
 *
 * @params - word_t word is the binary instruction to decode
 * @return - instruction_t* is a pointer to the decoded instruction
 */

instruction_t* decodeWord(word_t word){
    assert(word != NULL);

    instruction_t instruction;
    instruction_t* instructionPtr = &instruction;

    decodeInstructionType(instructionPtr, word);
    //Call functions to decode instruction

    return instructionPtr;
}

