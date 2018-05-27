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

    mul.A = getFlag(word, A_FLAG);

    mul.S = getFlag(word, S_FLAG);

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

//// DECODE ENTRY ////

/**
 * Decode Word (entry point to file)
 *
 * @param - word_t word is the binary instruction to decode
 * @return - instruction_t is the returned decoded instruction.
 */

instruction_t decodeWord(word_t word){

    instruction_t instruction;

    instruction.cond = getNibble(word, COND_START);
    decodeInstructionType(&instruction, word);

    return instruction;
}

int main(void){

    //TEST DECODE INSTRUCTION TYPE AND COND

    //Halt

    word_t halt_word = 0x0;
    instruction_t halt = decodeWord(halt_word);

    assert(halt.type == HAL);
    assert(halt.cond == 0);

    //DP

    word_t dp_word = 0xA2119871;
    instruction_t dp = decodeWord(dp_word);

    assert(dp.type == DP);
    assert(dp.cond == 0xA);

    word_t dp_word_2 = 0x112932C9;
    instruction_t dp_2 = decodeWord(dp_word_2);

    assert(dp_2.type == DP);
    assert(dp_2.cond == 0x1);

    //MUL

    word_t mul_word = 0xC033559C;
    instruction_t mul = decodeWord(mul_word);

    assert(mul.type == MUL);
    assert(mul.cond == 0xC);

    //BRANCH

    word_t brn_word = 0xBA0F0F0F;
    instruction_t brn = decodeWord(brn_word);

    assert(brn.type == BRN);
    assert(brn.cond == 0xB);

    return 0;
}
