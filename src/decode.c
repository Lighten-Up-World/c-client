/*
 *  The functions below are all used to decode the data processing
 *  instructions. Since there are many aspects to an instruction of this type
 *  we have broken it down into different structs, and we start the decoding
 *  process at the lowest level and work up.
 */

#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include "decode.h"
#include <assert.h>
#include <stdio.h>


/**
 *  Set the value of the operand when I is 0
 *
 *  @param opPtr: pointer to the operand
 *  @param word: the instruction word
 *  @return void: modifies the operand value of the instruction
 */
void decodeShiftedReg(operand_t* opPtr, word_t word) {

  opPtr->reg.type = getBits(word, OP_SHIFT_TYPE_START, OP_SHIFT_TYPE_END);
  opPtr->reg.shiftBy = getFlag(word, OP_SHIFTBY_FLAG);

  //Shift by register
  if(opPtr->reg.shiftBy){
    opPtr->reg.shift.shiftreg = (op_shift_register_t){
        .rs = getNibble(word, OP_START),
        .zeroPad = 0x0
    };

  // Shift by constant
  } else {
    opPtr->reg.shift.constant = (op_shift_const_t){
        .integer = getBits(word, OP_START, OP_IMM_START)
    };
  }
  opPtr->reg.rm = getNibble(word, REG_M_START);
}

/**
 *  Get the operand by decoding the word instruction
 *
 *  @param I: the immediate flag deciding how to decode the operand
 *  @param word: the instruction word
 *  @return a populated operand based on the data in word
 */
operand_t decodeOperand(flag_t I, word_t word){
  operand_t op;
  //Immediate
  if(I){
    op.imm = (op_immediate_t){.rotated.rotate = getNibble(word, OP_START),
        .rotated.value = getByte(word, OP_IMM_START)};

  //Register
  } else {
    decodeShiftedReg(&op, word);
  }
  return op;
}

/**
 *  Get the offset (operand) by decoding the word instruction
 *
 *  @param I: the immediate flag on how to decode the operand
 *  @param word: the instruction word
 *  @return a populated operand based on the data in word
 */
operand_t decodeOffset(flag_t I, word_t word){
  operand_t op;
  //Register
  if(I){
    decodeShiftedReg(&op, word);

  //Immediate
  } else {
    op.imm.fixed = getBits(word, OP_START,0);
  }
  return op;
}

/**
 *  Decodes a Data Processing instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the instruction word
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
void decodeDp(instruction_t* instructionPtr, word_t word) {
  assert(instructionPtr != NULL);

  dp_instruction_t dp;

  dp.padding = 0x0;
  dp.I = getFlag(word, I_FLAG);
  dp.opcode = getNibble(word, OPCODE_START);
  dp.S = getFlag(word, S_FLAG);
  dp.rn = getNibble(word, DP_RN_START);
  dp.rd = getNibble(word, DP_RD_START);
  dp.operand2 = decodeOperand(dp.I, word);

  instructionPtr->i.dp = dp;
}

/**
 *  Decodes a Multiplication Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the instruction word
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
void decodeMul(instruction_t* instructionPtr, word_t word){
    mul_instruction_t mul;

    mul.pad0 = 0x0;
    mul.A = getFlag(word, A_FLAG);
    mul.S = getFlag(word, S_FLAG);
    mul.rd = getNibble(word, MUL_RD_START);
    mul.rn = getNibble(word, MUL_RN_START);
    mul.rs = getNibble(word, REG_S_START);
    mul.pad9 = 0x9;
    mul.rm = getNibble(word, REG_M_START);

    instructionPtr->i.mul = mul;
}

/**
 *  Decode a Single Data Transfer Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the instruction word
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
void decodeSdt(instruction_t *instructionPtr, word_t word){
  sdt_instruction_t sdt;

  sdt.pad1 = 0x1;
  sdt.I = getFlag(word, I_FLAG);
  sdt.P = getFlag(word, P_FLAG);
  sdt.U = getFlag(word, U_FLAG);
  sdt.pad0 = 0x0;
  sdt.L = getFlag(word, L_FLAG);
  sdt.rn = getNibble(word, SDT_RN_START);
  sdt.rd = getNibble(word, SDT_RD_START);
  sdt.offset = decodeOffset(sdt.I, word);

  instructionPtr->i.sdt = sdt;
}

/**
 *  Decode a Branch Instruction
 *
 *  @param - instruction_t* instructionPtr is the pointer to the instruction
 *  @param - word_t word is the binary instruction
 *  @return - void, changes made to the instruction pointed to by i
 */
void decodeBrn(instruction_t *instructionPtr, word_t word) {
  brn_instruction_t brn;

  brn.padA = 0xA;
  brn.offset = getBits(word, 23, 0);
  instructionPtr->i.brn = brn;
}

//// HAL ////

/**
 *  Decode a Halt Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the binary instruction
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
void decodeHalt(instruction_t* instructionPtr, word_t word) {

  assert(instructionPtr != NULL);
  assert(word == 0x0000);

  hal_instruction_t hal;
  hal.pad0 = word;

  instructionPtr->i.hal = hal;

}

/**
 *  Decode Instruction Type
 *
 *  @param instructionPtr: is a pointer to the instruction_t
 *  @param word: is the binary instruction to decode
 *  @return void: modifies type field of the instruction pointed to by instructionPtr
 */
void decodeInstructionType(instruction_t* instructionPtr, word_t word){
    assert(instructionPtr != NULL);

    instruction_type_t instruction_type;

    if (word == 0x0){
        instruction_type = HAL;
        decodeHalt(instructionPtr, word);
    }  else {
        word_t selectionBits = getBits(word, INSTR_TYPE_START, INSTR_TYPE_END);
        word_t pad9;
        switch (selectionBits) {
            case 0x0:
                pad9 = getBits(word, MUL_TYPE_START, MUL_TYPE_END);
                if (!(pad9 ^ 0x9)) {
                  instruction_type = MUL;
                  decodeMul(instructionPtr, word);
                } else {
                  instruction_type = DP;
                  decodeDp(instructionPtr, word);
                }
                break;
            case 0x1:
                instruction_type = DP;
                decodeDp(instructionPtr, word);
                break;
            case 0x2:
            case 0x3:
              instruction_type = SDT;
              decodeSdt(instructionPtr, word);
              break;
            case 0x5:
                instruction_type = BRN;
                decodeBrn(instructionPtr, word);
                break;
            default: //Shouldn't be default makes errors really difficult to debug
                assert(false); //TODO: Add error code
                break;
        }
    }

    instructionPtr->type = instruction_type;
}

/**
 *  Decode Word (entry point to file)
 *
 *  @param word: is the binary instruction to decode
 *  @return the decoded instruction.
 */
instruction_t decodeWord(word_t word){
    instruction_t instruction;
    printf("\nWord: %08x\n", word);
    instruction.cond = getNibble(word, COND_START);
    printf("Word: %01x\n", instruction.cond);
    decodeInstructionType(&instruction, word);

    return instruction;
}
