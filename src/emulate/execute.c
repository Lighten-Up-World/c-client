/*
 *  Executes the decoded instruction and modifies the state based on the
 *  instruction
 */

#include "execute.h"

/**
 *  Check if the condition on the decoded instruction is met using the current
 *  state of the flags register (CPSR).
 *
 *  @param state: pointer to the current VM state
 *  @param cond: condition extracted from the instruction
 *  @return: 1 when condition is met, 0 if not
 */
int condition(emulate_state_t *state, byte_t cond) {
  byte_t flags = get_flags(state);
  switch (cond) {
    case EQ:
      return flags & Z;
    case NE:
      return !(flags & Z);
    case GE:
      return (flags & V) == ((flags & N) >> 3);
    case LT:
      return (flags & V) != ((flags & N) >> 3);
    case GT:
      return !(flags & Z) && ((flags & V) == ((flags & N) >> 3));
    case LE:
      return (flags & Z) || ((flags & V) != ((flags & N) >> 3));
    case AL:
      return 1;
    default:
      fprintf(stderr, "Invalid cond flag %x\n", cond);
      return 0;
  }
}

/**
 *  Evaluate the value of shifted reg and stores result.
 *  Called from evaluateOperand or from evaluateOffset
 *
 *  @param state: pointer to state of program
 *  @param op: operand/offset from evaluateOperand or evaluateOffset
 *  @param result: pointer to the result,
 *  so that changes can be made directly to it.
 */
void evaluate_shifted_reg(emulate_state_t *state, operand_t op, shift_result_t *result) {
  word_t rm = get_register(state, op.reg.rm);
  byte_t shiftAmount = 0;
     if (op.reg.shiftBy) { //Shift by register
    shiftAmount = get_byte(get_register(state, op.reg.shift.shiftreg.rs), 7);
  } else { //Shift by constant
    shiftAmount = op.reg.shift.constant.integer;
  }
  switch (op.reg.type) {
    case LSL:
      *result = l_shift_left_c(rm, shiftAmount);
      break;
    case LSR:
      *result = l_shift_right_c(rm, shiftAmount);
      break;
    case ASR:
      *result = a_shift_right_c(rm, shiftAmount);
      break;
    case ROR:
      *result = rotate_right_c(rm, shiftAmount);
      break;
    default:
      //TODO: Add proper error handling code
      exit(EXIT_FAILURE);
  }
}

/**
 * Evaluate the operand and returns the result
 *
 * @param state: pointer to state of program
 * @param I: flag that determines the meaning of operand
 * @param op: operand being evaluated
 * @return: result of evaluating the operand
 */
shift_result_t evaluate_operand(emulate_state_t *state, flag_t I,
                                operand_t op) {
  shift_result_t result;
  if (I) {

    result.value = left_pad_zeros(op.imm.rotated.value);
    result = rotate_right_c(result.value, op.imm.rotated.rotate * 2);

  } else {
    evaluate_shifted_reg(state, op, &result);
  }
  return result;
}

/**
 * Evaluate the offset and returns the result
 *
  * @param state: pointer to state of program
 * @param I: flag that determines the meaning of offset
 * @param op: offset being evaluated
 * @return: result of evaluating the offset
 */
shift_result_t evaluate_offset(emulate_state_t *state, flag_t I, operand_t op) {
  shift_result_t result;
  if (I) {
    evaluate_shifted_reg(state, op, &result);
  } else {
    result.carry = 0;
    result.value = op.imm.fixed;
  }
  return result;
}

/**
 *  Execute an instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: the instruction to execute
 *  @return: integer error code returned by a particular execute function
 */
int execute(emulate_state_t *state) {
  instruction_t *decoded = state->pipeline.decoded;
  if (decoded->type == HAL) {
    return execute_halt(state);
  }


  if (condition(state, decoded->cond)) {
    switch (decoded->type) {
      case DP:
        return execute_dp(state, decoded->i.dp);
      case MUL:
        return execute_mul(state, decoded->i.mul);
      case SDT:
        return execute_sdt(state, decoded->i.sdt);
      case BRN:
        return execute_brn(state, decoded->i.brn);
      default:
        fprintf(stderr, "Invalid type%x\n", decoded->type);
        return 2;
    }
  }
  return 0;
}

/**
 *  Execute Data Processing instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: DP instruction to execute
 *  @return: integer error code based on success of the function
 */
int execute_dp(emulate_state_t *state, dp_instruction_t instr) {
  shift_result_t barrel = evaluate_operand(state, instr.I, instr.operand2);
  word_t op2 = barrel.value;
  word_t result = 0;
  word_t rn = get_register(state, instr.rn);
  switch (instr.opcode) {
    case AND:
    case TST:
      result = rn & op2;
      break;
    case EOR:
    case TEQ:
      result = rn ^ op2;
      break;
    case SUB:
    case CMP:
      result = rn + negate(op2);
      break;
    case RSB:
      result = op2 + negate(rn);
      break;
    case ADD:
      result = rn + op2;
      break;
    case ORR:
      result = rn | op2;
      break;
    case MOV:
      result = op2;
      break;
  }


  if (instr.S) {
    byte_t flags = 0x0;
    switch (instr.opcode) {
      case AND:
      case TST:
      case EOR:
      case TEQ:
      case ORR:
      case MOV:
        flags |= C * barrel.carry;
        break;
      case SUB:
      case RSB:
      case CMP:
      case ADD:
        flags |= C * ((is_negative(rn) == is_negative(op2))
            != is_negative(result));
        break;
    }
    flags |= (N * is_negative(result));
    flags |= (Z * (result == 0));
         set_flags(state, flags);
       }

  if (instr.opcode != TST && instr.opcode != TEQ && instr.opcode != CMP) {
    set_register(state, instr.rd, result);
  }
  return 0;
}

/**
 *  Execute Multiply instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: the MUL instruction to execute
 *  @return: integer error code based on success of the function
 */
int execute_mul(emulate_state_t *state, mul_instruction_t instr) {
  // Cast the operands to 64 bit since this is the max result of A * B
  // where A, B are 32 bit
  uint64_t Rm = get_register(state, instr.rm);
  uint64_t Rs = get_register(state, instr.rs);
  uint64_t Rn = get_register(state, instr.rn);

  uint64_t res;
  if (instr.A) {
    res = Rm * Rs + Rn;
  } else {
    res = Rm * Rs;
  }

  // Mask to get only lower 32 bits of Rd, signed/unsigned does not affect these
  uint64_t mask = ~(UINT64_MAX - UINT32_MAX);
  uint32_t Rd = (u_int32_t)(res & mask);

  // Set flag bits
  if (instr.S) {
    byte_t flags = get_flags(state);

    // Set N flag
    byte_t N = (byte_t) get_bits(Rd, 31, 31);
    flags |= (N << (uint8_t) 3);

    // Set Z flag
    byte_t Z = 0;
    if (Rd == 0) {
      Z = 1;
    }
    flags |= (Z << (uint8_t) 2);

    set_flags(state, flags);
  }

  // Store result in Rd
  set_register(state, instr.rd, Rd);
  return 0;
}

/**
 *  Execute a branch instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: BRN instruction to execute
 *  @return: integer error code based on success of the function
 */
int execute_brn(emulate_state_t *state, brn_instruction_t instr) {
  int _status = EC_OK;
  word_t pc = get_pc(state);

  //Shift offset left by 2 bits
  word_t shiftedOffset = l_shift_left(instr.offset, 0x2);

  //Sign extend offset to 32 bits
  shiftedOffset |= (shiftedOffset >> 23) ? OFFSET_BITMASK : 0x0;

  //Assume that the offset takes into account the knowledge that the PC is
  // 8 bytes ahead of the instruction being executed.
  set_pc(state, pc + (int32_t) shiftedOffset);

  // Fetch new word at PC
  get_mem_word(state, get_pc(state), state->pipeline.fetched);
  _status = decode_word(state->pipeline.decoded, *state->pipeline.fetched);
  increment_pc(state);
  get_mem_word(state, get_pc(state), state->pipeline.fetched);
  return _status;
}

/**
 *  Execute Single Data Transfer instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: SDT instruction to execute
 *  @return: integer error code based on success of the function
 */
int execute_sdt(emulate_state_t *state, sdt_instruction_t instr) {
  shift_result_t barrel = evaluate_offset(state, instr.I, instr.offset);
  word_t offset = barrel.value;
  word_t rn = get_register(state, instr.rn);
  word_t data;


  if (instr.P) {
    //Pre-indexing

    if (instr.U) {
      rn += offset;
    } else {
      rn -= offset;
    }
    if (instr.L) {
      //Load from memory at address rn into reg rd.
      if (!get_mem_word(state, rn, &data)) {
        set_register(state, instr.rd, data);
      }
    } else {
      //Store contents of reg rd in memory at address rn.
      set_mem_word(state, rn, get_register(state, instr.rd));
    }
    return 0;
  } else {
    //Post-indexing
    if (instr.L) {
      //Load from memory at address rn into reg rd.
      if (!get_mem_word(state, rn, &data)) {
        set_register(state, instr.rd, data);
      }

    } else {
      //Store contents of reg rd in memory at address rn.
      set_mem_word(state, rn, get_register(state, instr.rd));
    }
    if (instr.U) {
      rn += offset;
    } else {
      rn -= offset;
    }

    //Change contents of reg rn (the base register)
    set_register(state, instr.rn, rn);
    return 0;
  }
}

/**
 *  Execute halt instruction
 *
 *  @param state: pointer to the program_state state
 *  @param instr: the halt instruction to execute
 *  @return: integer error code based on success of the function
 */
int execute_halt(emulate_state_t *state) {
  print_state(state);
  return 0;
}
