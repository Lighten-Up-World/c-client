/*
 *  The functions below are all used to decode the different types of
 *  instructions. Since there are many aspects to an instruction,
 *  we have broken them down into different structs and we start the decoding
 *  process at the lowest level and work up.
 */

#include "decode.h"

/**
 *  Set the value of the operand when I is 0
 *
 *  @param opPtr: pointer to the operand
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_shifted_reg(operand_t *op, word_t word) {

  op->reg.type = get_bits(word, OP_SHIFT_TYPE_START, OP_SHIFT_TYPE_END);
  op->reg.shiftBy = get_flag(word, OP_SHIFTBY_FLAG);

  // Shift by register
  if (op->reg.shiftBy) {
    op->reg.shift.shiftreg = (op_shift_register_t) {
        .rs = get_nibble(word, OP_START),
        .zeroPad = 0x0
    };

    // Shift by constant
  } else {
    op->reg.shift.constant = (op_shift_const_t) {
        .integer = get_bits(word, OP_START, OP_IMM_START)
    };
  }
  op->reg.rm = get_nibble(word, REG_M_START);
  return EC_OK;
}

/**
 *  Get the operand by decoding the word instruction
 *
 *  @param I: immediate flag deciding how to decode the operand
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_operand(flag_t I, word_t word, operand_t *op) {
  // Immediate
  if (I) {
    op->imm = (op_immediate_t) {.rotated.rotate = get_nibble(word, OP_START),
        .rotated.value = get_byte(word, OP_IMM_START)};
  }
  // Register
  else {
    return decode_shifted_reg(op, word);
  }
  return EC_OK;
}

/**
 *  Get the offset (operand) by decoding the word instruction
 *
 *  @param I: immediate flag on how to decode the operand
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_offset(flag_t I, word_t word, operand_t *op) {
  // Register
  if (I) {
    return decode_shifted_reg(op, word);
  }
  // Immediate
  else {
    op->imm.fixed = get_bits(word, OP_START, 0);
  }
  return EC_OK;
}


/**
 *  Decodes a Data Processing instruction
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_dp(instruction_t *instr, word_t word) {
  assert(instr != NULL);

  instr->i.dp.padding = 0x0;
  instr->i.dp.I = get_flag(word, I_FLAG);
  instr->i.dp.opcode = get_nibble(word, OPCODE_START);
  instr->i.dp.S = get_flag(word, S_FLAG);
  instr->i.dp.rn = get_nibble(word, DP_RN_START);
  instr->i.dp.rd = get_nibble(word, DP_RD_START);
  return decode_operand(instr->i.dp.I, word, &instr->i.dp.operand2);

}

/**
 *  Decodes a Multiplication Instruction
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_mul(instruction_t *instr, word_t word) {
  assert(instr != NULL);

  instr->i.mul.pad0 = 0x0;
  instr->i.mul.A = get_flag(word, A_FLAG);
  instr->i.mul.S = get_flag(word, S_FLAG);
  instr->i.mul.rd = get_nibble(word, MUL_RD_START);
  instr->i.mul.rn = get_nibble(word, MUL_RN_START);
  instr->i.mul.rs = get_nibble(word, REG_S_START);
  instr->i.mul.pad9 = 0x9;
  instr->i.mul.rm = get_nibble(word, REG_M_START);

  return EC_OK;
}

/**
 *  Decode a Single Data Transfer Instruction
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_sdt(instruction_t *instr, word_t word) {
  assert(instr != NULL);

  instr->i.sdt.pad1 = 0x1;
  instr->i.sdt.I = get_flag(word, I_FLAG);
  instr->i.sdt.P = get_flag(word, P_FLAG);
  instr->i.sdt.U = get_flag(word, U_FLAG);
  instr->i.sdt.pad0 = 0x0;
  instr->i.sdt.L = get_flag(word, L_FLAG);
  instr->i.sdt.rn = get_nibble(word, SDT_RN_START);
  instr->i.sdt.rd = get_nibble(word, SDT_RD_START);
  return decode_offset(instr->i.sdt.I, word, &instr->i.sdt.offset);

}

/**
 *  Decode a Branch Instruction
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_brn(instruction_t *instr, word_t word) {
  assert(instr != NULL);

  instr->i.brn.padA = 0xA;
  instr->i.brn.offset = get_bits(word, 23, 0);
  return EC_OK;
}

/**
 *  Decode a Halt Instruction
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_halt(instruction_t *instr, word_t word) {
  assert(instr != NULL);

  if (word != 0x0000) {
    return EC_INVALID_PARAM;
  }

  instr->i.hal.pad0 = word;
  return EC_OK;
}

/**
 *  Decode Instruction Type
 *
 *  @param instr: pointer to the instruction
 *  @param word: instruction word to decode
 *  @return: integer error code based on success of function
 */
int decode_instruction_type(instruction_t *instr, word_t word) {
  assert(instr != NULL);
  int _status = EC_OK;

  instruction_type_t instruction_type;

  if (word == 0x0) {
    instruction_type = HAL;
    _status = decode_halt(instr, word);
  } else {
    word_t selectionBits = get_bits(word, INSTR_TYPE_START, INSTR_TYPE_END);
    word_t pad9;
    switch (selectionBits) {
      case 0x0:
        pad9 = get_bits(word, MUL_TYPE_START, MUL_TYPE_END);
        if (!(pad9 ^ 0x9)) {
          instruction_type = MUL;
          _status = decode_mul(instr, word);
        } else {
          instruction_type = DP;
          _status = decode_dp(instr, word);
        }
        break;
      case 0x1:
        instruction_type = DP;
        _status = decode_dp(instr, word);
        break;
      case 0x2:
      case 0x3:
        instruction_type = SDT;
        _status = decode_sdt(instr, word);
        break;
      case 0x5:
        instruction_type = BRN;
        _status = decode_brn(instr, word);
        break;
      default:
        return EC_UNSUPPORTED_OP;
    }
  }

  instr->type = instruction_type;
  return _status;
}

/**
 *  Decode Word (entry point to file)
 *
 *  @param instr: instruction to load with the data in the word
 *  @param word: binary instruction to decode
 *  @return: integer error code based on success of function
 */
int decode_word(instruction_t *instr, word_t word) {
  instr->cond = get_nibble(word, COND_START);
  return decode_instruction_type(instr, word);
}
