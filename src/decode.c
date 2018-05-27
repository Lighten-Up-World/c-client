#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include "decode.h"
#include <assert.h>
#include <stdio.h>

//// DP ////


//// MUL ////

/**
 * Decode Multiplication Instruction
 *
 * @param - instruction_t* instructionPtr is the pointer to the instruction
 * @param - word_t word is the binary instruction
 * @return - void, changes made to the instruction pointed to by instructionPtr
 */

void decodeMul(instruction_t* instructionPtr, word_t word){

    mul_instruction_t mul;

    mul.pad0 = 0x0;

    mul.A = getFlag(word, MUL_ACC);

    mul.S = getFlag(word, MUL_SET);

    mul.rd = getNibble(word, REG_1_START);

    mul.rn = getNibble(word, REG_2_START);

    mul.rs = getNibble(word, REG_S_START);

    mul.pad9 = 0x9;

    mul.rm = getNibble(word, REG_M_START);

    instructionPtr->i.mul = mul;
}

//// SDT ////


//// BRN ////


//// HAL ////


//// INSTRUCTION TYPE ////

/**
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
        //decodeHal(instructionPtr, word);
    }else {
        word_t selectionBits = getBits(word, INSTR_TYPE_START, INSTR_TYPE_END);
        word_t pad9;
        switch (selectionBits) {
            case 0x0:
                pad9 = getBits(word, MUL_TYPE_START, MUL_TYPE_END);
                if (pad9 ^ 0x9) {
                    instruction_type = DP;
                    //decodeDp(instructionPtr, word);
                } else {
                    instruction_type = MUL;
                    decodeMul(instructionPtr, word);
                }
                break;
            case 0x1:
                instruction_type = DP;
                //decodeDp(instructionPtr, word);
                break;
            case 0x5:
                instruction_type = BRN;
                //decodeBrn(instructionPtr, word);
                break;
            default:
                instruction_type = SDT;
                //decodeSdt(instructionPtr, word);
                break;
        }
    }

    instructionPtr->type = instruction_type;
}

//// INSTRUCTION CONDITION ////

/**
 * Decode Instruction Condition
 *
 * @param - instruction_t* instructionPtr is a pointer to the instruction_t
 * @param - word_t word is the binary instruction to decode
 * @return - void, all changes occur directly to instruction_t in memory.
 */

void decodeCondition(instruction_t* instructionPtr, word_t word){

    instructionPtr->cond = getNibble(word, COND_START);

}


//// DECODE ENTRY ////

/**
 * Decode Word (entry point to file)
 *
 * @param - word_t word is the binary instruction to decode
 * @return - instruction_t is the returned decoded instruction.
 */

instruction_t decodeWord(word_t word){

    instruction_t instruction;
    instruction_t* instructionPtr = &instruction;

    decodeCondition(instructionPtr, word);
    decodeInstructionType(instructionPtr, word);

    return instruction;
}

int main(void){

    //TEST DECODE INSTRUCTION TYPE AND COND

    //Halt

    word_t halt_word = 0b00000000000000000000000000000000;
    instruction_t halt = decodeWord(halt_word);

    assert(halt.type == HAL);
    assert(halt.cond == 0);

    //DP

    word_t dp_word = 0b10100010000100011001100001110001;
    instruction_t dp = decodeWord(dp_word);

    assert(dp.type == DP);
    assert(dp.cond == 0b1010);

    word_t dp_word_2 = 0b00010001001010010011001011001001;
    instruction_t dp_2 = decodeWord(dp_word_2);

    assert(dp_2.type == DP);
    assert(dp_2.cond == 0b0001);

    //MUL

    word_t mul_word = 0b11000000001100110101010110011100;
    instruction_t mul = decodeWord(mul_word);

    assert(mul.type == MUL);
    assert(mul.cond == 0b1100);

    //BRANCH

    word_t brn_word = 0b10111010000011110000111100001111;
    instruction_t brn = decodeWord(brn_word);

    assert(brn.type == BRN);
    assert(brn.cond == 0b1011);

    return 0;
}


