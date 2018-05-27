#include "arm.h"
#include "instructions.h"
#include "bitops.h"
#include "decode.h"
#include <assert.h>
#include <stdio.h>

/**
 *  Get the operand_t by decoding the word instruction
 *
 *  @param I: the immediate flag on how to decode the operand
 *  @param word: the instruction word
 *  @return A fully populated operand type based on data in word
 */
operand_t decodeOperand(flag_t I, word_t word){
  operand_t op;
  if(I){ //Immediate
    op.imm = (op_immediate_t){.rotated.rotate = getNibble(word, OP_START),
                                    .rotated.value = getByte(word, OP_IMM_START)};
  }
  else { //Register
    op.reg.type = getBits(word, OP_SHIFT_TYPE_START, OP_SHIFT_TYPE_END);
    op.reg.shiftBy = getFlag(word, OP_SHIFTBY_FLAG);
    if(op.reg.shiftBy){ //Shift by register
      op.reg.shift.shiftreg = (op_shift_register_t){
        .rs = getNibble(word, OP_START),
        .zeroPad = 0x0
      };
    }
    else{ // Shift by constant
      op.reg.shift.constant = (op_shift_const_t){
        .integer = getBits(word, OP_START, OP_IMM_START)
      };
    }
    op.reg.rm = getNibble(word, REG_M_START);
  }
  return op;
}

//// DP ////

/**
 * The functions below are all used to decode the data processing
 * instructions. Since there are many aspects to an instruction of this type
 * we have broken it down into different structs, and we start the decoding
 * process at the lowest level and work up.
 */

/**
 * @param operand_t* operand2Ptr - is a pointer to operand_2 of the dp instr.
 * @param word_t word - the 32 bit memory word that comprises of the
 * instruction in binary
 * @return void - we alter the value of operand2 (in dp) using it's pointer.
 */
void decodeImm(operand_t* operand2Ptr, word_t word) {

  assert(operand2Ptr != NULL);
  assert(word != NULL);

  op_immediate_t opImm;

  byte_t rotate = (byte_t) getBits(word,OP2_ROTATE_ST, OP2_ROTATE_END);
  opImm.rotate = rotate;

  byte_t immVal = (byte_t) getBits(word, OP2_IMM_START, OP2_IMM_END);
  opImm.immediate = immVal;

  operand2Ptr->immediate = opImm;
}

void decodeShiftByReg(op_shiftreg_t* opShiftregPtr, word_t word) {
  //TODO ADD assertions

  op_shift_register_t shiftByReg;
  op_shift_register_t* shiftByRegPtr = &shiftByReg;

  byte_t rs = (byte_t) getBits(word, RS_START, RS_END);
  shiftByReg.Rs = rs;

  shiftByReg.zeroPad = 0x0;

  shift_type_t shiftType = (shift_type_t) getBits(word, SHIFT_TYPE_START,
                                                  SHIFT_TYPE_END);
  shiftByReg.type = shiftType;

  shiftByReg.onePad = 0x1;

  opShiftregPtr->shift.shiftreg = shiftByReg;
}

void decodeShiftByConst(op_shiftreg_t* opShiftregPtr, word_t word) {
  //TODO Add assertions

  op_shift_const_t shiftByConst;
  op_shift_const_t* shiftByConstPtr = &shiftByConst;

  byte_t integer = (byte_t) getBits(word, SHFT_VAL_START, SHIFT_VAL_END);
  shiftByConst.integer = integer;

  shift_type_t shiftType = (shift_type_t) getBits(word, SHIFT_TYPE_START,
                                                  SHIFT_TYPE_END);
  shiftByConst.type = shiftType;

  shiftByConst.padding = 0x0;

  opShiftregPtr->shift.constant = shiftByConst;
}

/**
 * @param operand_t* operand2Ptr - is a pointer to operand_2 of the dp instr.
 * @param word_t word - the 32 bit memory word that comprises of the
 * instruction in binary
 * @return void - we alter the value of operand2 (in dp) using it's pointer.
 */
void decodeShiftReg(operand_t* operand2Ptr, word_t word) {
 //TODO Add assertions

 op_shiftreg_t opShiftreg;
 op_shiftreg_t* opShiftregPtr = &opShiftreg;

 byte_t rm = (byte_t) getBits(word, OP2_RM_START, OP2_RM_END);
 opShiftreg.rm = rm;

 byte_t shiftSelectBit = (byte_t) getBits(word, SHIFT_SELECT, SHIFT_SELECT);
 if (shiftSelectBit) {
   decodeShiftByReg(opShiftregPtr, word);
 } else {
   decodeShiftByConst(opShiftregPtr, word);
 }

 operand2Ptr->shiftreg = opShiftreg;

}
/**
 * @param - dp_instruction_t* dp is a pointer to the data
 * processing instruction to decode. The method will need to determine
 * whether operand2 is an immediate value or not.
 * @return - void, the method will modify the value of dp in memory using
 * it's pointer.
 *
 */
void decodeOperand2(dp_instruction_t* dpPtr, word_t word) {

  assert(dpPtr != NULL);
  assert(word != NULL);

  operand_t operand2;
  operand_t* operand2Ptr = &operand2;

  if (dpPtr->I) {
    decodeImm(operand2Ptr, word);
  } else {
    decodeShiftReg(operand2Ptr, word);
  }

  dpPtr->operand2 = operand2;
}

/**
 * @param - instruction_t* instructionPtr is a pointer to the instruction to
 * be decoded.
 * @param -  word_t word is the binary instruction.
 * @return - void, we modify the value of the instruction using the pointer.
 */

void decodeDp(instruction_t* instructionPtr, word_t word) {

  assert(instructionPtr != NULL);
  assert(word != NULL);

  dp_instruction_t dp; // create dp instruction
  dp_instruction_t* dpPtr = &dp; // create pointer to dp instruction

  dp.

  decodeOperand2(dpPtr, word);

  instructionPtr->i.dp = dp; // set decoded instruction to value of the
                             // instruction of the pointer.
}

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

/**
 * Decode Single Data transfer Instruction
 *
 * @param - instruction_t* instructionPtr is the pointer to the instruction
 * @param - word_t word is the binary instruction
 * @return - void, changes made to the instruction pointed to by i
 */
void decodeSdt(instruction_t *instructionPtr, word_t word){
  sdt_instruction_t sdt;

  sdt.pad1 = 0x1;
  sdt.I = getFlag(word, I_FLAG);
  sdt.P = getFlag(word, P_FLAG);
  sdt.U = getFlag(word, U_FLAG);
  sdt.pad0 = 0x0;
  sdt.L = getFlag(word, L_FLAG);
  sdt.rn = getNibble(word, REG_1_START);
  sdt.rd = getNibble(word, REG_2_START);
  sdt.offset = decodeOperand(sdt.I, word);

  instructionPtr->i.sdt = sdt;
}

//// BRN ////


//// HAL ////

void decodeHalt(instruction_t* instructionPtr, word_t word) {

  assert(instructionPtr != NULL);
  assert(word != NULL);
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

    instruction_type_t instruction_type;

    if (word == 0x0){
        instruction_type = HAL;
        //decodeHalt(instructionPtr, word);
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
