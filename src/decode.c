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
    op.imm = (op_immediate_t){.rotate = getNibble(word, OP_START),
                                    .value = getByte(word, OP_IMM_START)};
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

/*
 *  Decode Single Data transfer Instruction
 *
 *  @param - instruction_t* instructionPtr is the pointer to the instruction
 *  @param - word_t word is the binary instruction
 *  @return - void, changes made to the instruction pointed to by i
 */
void decodeBrn(instruction_t *instructionPtr, word_t word) {
  brn_instruction_t brn;

  brn.pad5 = 0b101;
  brn.pad0 = 0b0;
  brn.offset = getBits(word, 23, 0);

  instructionPtr->i.brn = brn;
}

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
                decodeBrn(instructionPtr, word);
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


/*int main() {
  instruction_t inst;
  word_t w1 = 0b10101010111111111111111111111111;
  word_t w2 = 0b11111010111111111111111111111111;
  word_t w3 = 0b00001010000000000000000000000000;
  word_t w4 = 0b00111010100000000000000000000001;

  inst = decodeWord(w1);
  assert(inst.type == BRN);
  assert(inst.cond == 0b1010);
  assert(inst.i.brn.offset == 0b111111111111111111111111);
  assert(inst.i.brn.pad5 == 0b101);
  assert(inst.i.brn.pad0 == 0b0);

  inst = decodeWord(w2);
  assert(inst.type == BRN);
  assert(inst.cond == 0b1111);
  assert(inst.i.brn.offset == 0b111111111111111111111111);
  assert(inst.i.brn.pad5 == 0b101);
  assert(inst.i.brn.pad0 == 0b0);

  inst = decodeWord(w3);
  assert(inst.type == BRN);
  assert(inst.cond == 0b0000);
  assert(inst.i.brn.offset == 0b000000000000000000000000);
  assert(inst.i.brn.pad5 == 0b101);
  assert(inst.i.brn.pad0 == 0b0);

  inst = decodeWord(w4);
  assert(inst.type == BRN);
  assert(inst.cond == 0b0011);
  assert(inst.i.brn.offset == 0b100000000000000000000001);
  assert(inst.i.brn.pad5 == 0b101);
  assert(inst.i.brn.pad0 == 0b0);

  return 0;
}*/