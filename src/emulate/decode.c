/*
 *  The functions below are all used to decode the data processing
 *  instructions. Since there are many aspects to an instruction of this type
 *  we have broken it down into different structs, and we start the decoding
 *  process at the lowest level and work up.
 */

#include "utils/arm.h"
#include "utils/instructions.h"
#include "utils/bitops.h"
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
void decode_shifted_reg(operand_t *opPtr, word_t word) {

  opPtr->reg.type = get_bits(word, OP_SHIFT_TYPE_START, OP_SHIFT_TYPE_END);
  opPtr->reg.shiftBy = get_flag(word, OP_SHIFTBY_FLAG);

  //Shift by register
  if (opPtr->reg.shiftBy) {
    opPtr->reg.shift.shiftreg = (op_shift_register_t) {
        .rs = get_nibble(word, OP_START),
        .zeroPad = 0x0
    };

    // Shift by constant
  } else {
    opPtr->reg.shift.constant = (op_shift_const_t) {
        .integer = get_bits(word, OP_START, OP_IMM_START)
    };
  }
  opPtr->reg.rm = get_nibble(word, REG_M_START);
}

/**
 *  Get the operand by decoding the word instruction
 *
 *  @param I: the immediate flag deciding how to decode the operand
 *  @param word: the instruction word
 *  @return a populated operand based on the data in word
 */
operand_t decode_operand(flag_t I, word_t word) {
  operand_t op;
  //Immediate
  if (I) {
    op.imm = (op_immediate_t) {.rotated.rotate = get_nibble(word, OP_START),
        .rotated.value = get_byte(word, OP_IMM_START)};

    //Register
  } else {
    decode_shifted_reg(&op, word);
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
operand_t decode_offset(flag_t I, word_t word) {
  operand_t op;
  //Register
  if (I) {
    decode_shifted_reg(&op, word);

    //Immediate
  } else {
    op.imm.fixed = get_bits(word, OP_START, 0);
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
  void decode_dp(instruction_t *instructionPtr, word_t word) {
    assert(instructionPtr != NULL);

    dp_instruction_t dp;

    dp.padding = 0x0;
    dp.I = get_flag(word, I_FLAG);
    dp.opcode = get_nibble(word, OPCODE_START);
    dp.S = get_flag(word, S_FLAG);
    dp.rn = get_nibble(word, DP_RN_START);
    dp.rd = get_nibble(word, DP_RD_START);
    dp.operand2 = decode_operand(dp.I, word);

    instructionPtr->i.dp = dp;
  }

/**
 *  Decodes a Multiplication Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the instruction word
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
  void decode_mul(instruction_t *instructionPtr, word_t word) {
    mul_instruction_t mul;

    mul.pad0 = 0x0;
    mul.A = get_flag(word, A_FLAG);
    mul.S = get_flag(word, S_FLAG);
    mul.rd = get_nibble(word, MUL_RD_START);
    mul.rn = get_nibble(word, MUL_RN_START);
    mul.rs = get_nibble(word, REG_S_START);
    mul.pad9 = 0x9;
    mul.rm = get_nibble(word, REG_M_START);

    instructionPtr->i.mul = mul;
  }

/**
 *  Decode a Single Data Transfer Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the instruction word
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
  void decode_sdt(instruction_t *instructionPtr, word_t word) {
    sdt_instruction_t sdt;

    sdt.pad1 = 0x1;
    sdt.I = get_flag(word, I_FLAG);
    sdt.P = get_flag(word, P_FLAG);
    sdt.U = get_flag(word, U_FLAG);
    sdt.pad0 = 0x0;
    sdt.L = get_flag(word, L_FLAG);
    sdt.rn = get_nibble(word, SDT_RN_START);
    sdt.rd = get_nibble(word, SDT_RD_START);
    sdt.offset = decode_offset(sdt.I, word);

    instructionPtr->i.sdt = sdt;
  }

/**
 *  Decode a Branch Instruction
 *
 *  @param - instruction_t* instructionPtr is the pointer to the instruction
 *  @param - word_t word is the binary instruction
 *  @return - void, changes made to the instruction pointed to by i
 */
  void decode_brn(instruction_t *instructionPtr, word_t word) {
    brn_instruction_t brn;

    brn.padA = 0xA;
    brn.offset = get_bits(word, 23, 0);
    instructionPtr->i.brn = brn;
  }

/**
 *  Decode a Halt Instruction
 *
 *  @param instructionPtr: a pointer to the instruction
 *  @param word: the binary instruction
 *  @return void: modifies the instruction pointed to by instructionPtr
 */
  void decode_halt(instruction_t *instructionPtr, word_t word) {

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
  void decode_instruction_type(instruction_t *instructionPtr, word_t word) {
    assert(instructionPtr != NULL);

    instruction_type_t instruction_type;

    if (word == 0x0) {
      instruction_type = HAL;
      decode_halt(instructionPtr, word);
    } else {
      word_t selectionBits = get_bits(word, INSTR_TYPE_START, INSTR_TYPE_END);
      word_t pad9;
      switch (selectionBits) {
        case 0x0:
          pad9 = get_bits(word, MUL_TYPE_START, MUL_TYPE_END);
          if (!(pad9 ^ 0x9)) {
            instruction_type = MUL;
            decode_mul(instructionPtr, word);
          } else {
            instruction_type = DP;
            decode_dp(instructionPtr, word);
          }
          break;
        case 0x1:
          instruction_type = DP;
          decode_dp(instructionPtr, word);
          break;
        case 0x2:
        case 0x3:
          instruction_type = SDT;
          decode_sdt(instructionPtr, word);
          break;
        case 0x5:
          instruction_type = BRN;
          decode_brn(instructionPtr, word);
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
  instruction_t decode_word(word_t word) {
    instruction_t instruction;
    DEBUG_PRINT("\nWord: %08x\n", word);
    instruction.cond = get_nibble(word, COND_START);
    DEBUG_PRINT("Word: %01x\n", instruction.cond);
    decode_instruction_type(&instruction, word);

    return instruction;
  }
