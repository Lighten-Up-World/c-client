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

  byte_t conditionBits = (byte_t) getBits(word, COND_START, COND_END);
  dp.cond = conditionBits;

  dp.padding = 0x0;

  flag_t immBit = (flag_t) getBits(word, DP_I, DP_I);
  dp.I = immBit;

  byte_t opcode = (byte_t) getBits(word, OPCODE_START, OPCODE_END);
  dp.opcode = opcode; // can you create this enum from the byte??

  flag_t sBit = (flag_t) getBits(word, DP_S, DP_S);
  dp.S = sBit;

  byte_t rn = (byte_t) getBits(word, RN_START, RN_END);
  dp.rn = rn;

  byte_t rd = (byte_t) getBits(word, RD_START, RD_END);
  dp.rd = rd;

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

