#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include "decode.h"
#include <assert.h>
#include <stdio.h>

//// DP ////

/**
 * The functions below are all used to decode the data processing
 * instructions. Since there are many aspects to an instruction of this type
 * we have broken it down into different structs, and we start the decoding
 * process at the lowest level and work up.
 */

/**
 * set the value of the operand when I is 0.
 *
 * @param opPtr - pointer to the operand
 * @param word - the instruction word
 * @return void - modifies the operand value of the instruction
 */
void decodeShiftedReg(operand_t* opPtr, word_t word) {

  opPtr->reg.type = getBits(word, OP_SHIFT_TYPE_START, OP_SHIFT_TYPE_END);
  opPtr->reg.shiftBy = getFlag(word, OP_SHIFTBY_FLAG);

  if(opPtr->reg.shiftBy){ //Shift by register
    opPtr->reg.shift.shiftreg = (op_shift_register_t){
        .rs = getNibble(word, OP_START),
        .zeroPad = 0x0
    };
  }
  else{ // Shift by constant
    opPtr->reg.shift.constant = (op_shift_const_t){
        .integer = getBits(word, OP_START, OP_IMM_START)
    };
  }
  opPtr->reg.rm = getNibble(word, REG_M_START);
}


/**
 *  Get the operand_t by decoding the word instruction
 *
 *  @param flag_t I - the immediate flag on how to decode the operand
 *  @param word_t word - the instruction word
 *  @return A fully populated operand type based on data in word
 */
operand_t decodeOperand(flag_t I, word_t word){
  operand_t op;
  operand_t* opPtr = &op;
  if(I){ //Immediate
    op.imm = (op_immediate_t){.rotated.rotate = getNibble(word, OP_START),
        .rotated.value = getByte(word, OP_IMM_START)};
  }
  else { //Register
    decodeShiftedReg(opPtr, word);
  }
  return op;
}


/**
 * Decode Data Processing instruction
 *
 * @param instruction_t* instructionPtr - a pointer to the instruction to
 * be decoded.
 * @param word_t word - the binary instruction.
 * @return void - we modify the value of the instruction using the pointer.
 */

void decodeDp(instruction_t* instructionPtr, word_t word) {

  assert(instructionPtr != NULL);

  dp_instruction_t dp; // create dp instruction
  dp_instruction_t* dpPtr = &dp; // create pointer to dp instruction

  dp.padding = 0x0;
  dp.I = getFlag(word, I_FLAG);
  dp.opcode = getNibble(word, OPCODE_START);
  dp.S = getFlag(word, S_FLAG);
  dp.rn = getNibble(word, DP_RN_START);
  dp.rd = getNibble(word, DP_RD_START);
  dp.operand2 = decodeOperand(dp.I, word);

  instructionPtr->i.dp = dp; // set decoded instruction to value of the
                             // instruction of the pointer.
}

//// MUL ////

/**
 * Decode Multiplication Instruction
 *
 * @param instruction_t* instructionPtr - is the pointer to the instruction
 * @param word_t word - is the binary instruction
 * @return void - changes made to the instruction pointed to by instructionPtr
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
 * Decode Single Data transfer Instruction
 *
 * @param instruction_t* instructionPtr - the pointer to the instruction
 * @param word_t word - the binary instruction
 * @return void - changes made to the instruction pointed to by i
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
  sdt.offset = decodeOperand(sdt.I, word);

  instructionPtr->i.sdt = sdt;
}

//// BRN ////


//// HAL ////

/**
 * Decode Halt Instruction
 *
 * @param instructionPtr - the pointer to the instruction
 * @param word - the binary instruction
 */
void decodeHalt(instruction_t* instructionPtr, word_t word) {

  assert(instructionPtr != NULL);
  assert(word == 0x0000);

  hal_instruction_t hal;
  hal.pad0 = word;

  instructionPtr->i.hal = hal;

}

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

    instruction_type_t instruction_type;

    if (word == 0x0){
        instruction_type = HAL;
        decodeHalt(instructionPtr, word);
    }else {
        word_t selectionBits = getBits(word, INSTR_TYPE_START, INSTR_TYPE_END);
        word_t pad9;
        switch (selectionBits) {
            case 0x0:
                pad9 = getBits(word, MUL_TYPE_START, MUL_TYPE_END);
                if (pad9 ^ 0x9) {
                    instruction_type = DP;
                    decodeDp(instructionPtr, word);
                } else {
                    instruction_type = MUL;
                    decodeMul(instructionPtr, word);
                }
                break;
            case 0x1:
                instruction_type = DP;
                decodeDp(instructionPtr, word);
                break;
            case 0x5:
                instruction_type = BRN;
                //decodeBrn(instructionPtr, word);
                break;
            default:
                instruction_type = SDT;
                decodeSdt(instructionPtr, word);
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
