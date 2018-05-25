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

    assert(instructionPtr != NULL);
    assert(word != NULL);

    mul_instruction_t mul;

    byte_t conditionBits = (byte_t) getBits(word, COND_START, COND_END);
    mul.cond = conditionBits;

    mul.pad0 = 0x0;

    flag_t accBit = (flag_t) getBits(word, MUL_ACC, MUL_ACC);
    mul.A = accBit;

    flag_t setCondBit = (flag_t) getBits(word, MUL_SET, MUL_SET);
    mul.S = setCondBit;

    reg_address_t regDest = (reg_address_t) getBits(word, REG_1_START, REG_1_END);
    mul.Rd = regDest;

    reg_address_t regN = (reg_address_t) getBits(word, REG_2_START, REG_2_END);
    mul.Rn = regN;

    reg_address_t regS = (reg_address_t) getBits(word, REG_S_START, REG_S_END);
    mul.Rs = regS;

    mul.pad9 = 0x9;

    reg_address_t regM = (reg_address_t) getBits(word,REG_M_START,REG_M_END);
    mul.Rm = regM;

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

    assert(instructionPtr != NULL);
    assert(word != NULL);

    instruction_type_t instruction_type;

    if (word == 0x0){
        instruction_type = HAL;
    }else {
        word_t selectionBits = getBits(word, INSTR_TYPE_START, INSTR_TYPE_END);
        word_t pad9;
        switch (selectionBits) {
            case 0x0:
                pad9 = getBits(word, MUL_TYPE_START, MUL_TYPE_END);
                if (pad9 ^ 0x9) {
                    instruction_type = DP;
                } else {
                    instruction_type = MUL;
                    decodeMul(instructionPtr, word);
                }
                break;
            case 0x1:
                instruction_type = MUL;
                decodeMul(instructionPtr, word);
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

/**
 * Decode Word (entry point to file)
 *
 * @param - word_t word is the binary instruction to decode
 * @return - instruction_t is the returned decoded instruction.
 */

instruction_t decodeWord(word_t word){

    assert(word != NULL);

    instruction_t instruction;
    instruction_t* instructionPtr = &instruction;

    decodeInstructionType(instructionPtr, word);

    return instruction;
}

