#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "bitops.h"
#include "register.h"
#include "io.h"
#include "instructions.h"
#include "arm.h"

/**
* Check if the condition on the decoded instruction is met using the current
* state of the flags register (CPSR).
*
* @param state: Pointer to the current VM state
* @param cond: The condition extracted from the instructionn
* @return 1 when condition is met, 0 if not.
*/
int condition(state_t *state, byte_t cond){
  byte_t flags = getFlags(state);

  switch(cond){
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
      //free_state();
      return 0;
  }
}

/**
 * Evaluates the value of shifted reg and stores result.
 * Called from evaluateOperand or from evaluateOffset
 *
 * @param - state_t *state is pointer to state of program
 * @param - operand_t op is the operand/offset from evaluateOperand or from evaluateOffset
 * @param - shift_result_t *result is the pointer to the result, so that changes can be made directly to it.
 */
void evaluateShiftedReg(state_t *state, operand_t op, shift_result_t *result){
  word_t rm = getRegister(state, op.reg.rm);
  byte_t shiftAmount = 0;
  if (op.reg.shiftBy){ //Shift by register
    shiftAmount = getByte(getRegister(state, op.reg.shift.shiftreg.rs), 7);
  }else{ //Shift by constant
    shiftAmount = op.reg.shift.constant.integer;
  }
  switch(op.reg.type){
    case LSL:
      *result = lShiftLeftC(rm, shiftAmount);
      break;
    case LSR:
      *result = lShiftRightC(rm, shiftAmount);
      break;
    case ASR:
      *result = aShiftRightC(rm, shiftAmount);
      break;
    case ROR:
      *result = rotateRightC(rm, shiftAmount);
      break;
    default:
      //TODO: Add proper error handling code
      exit(EXIT_FAILURE);
  }
}

/**
 * Evaluates the operand and returns the result from
 *
 * @param - state_t *state is pointer to the
 */
shift_result_t evaluateOperand(state_t *state, flag_t I, operand_t op){
  shift_result_t result;
  if(I){ //Immediate value
    result.value = leftPadZeros(op.imm.rotated.value);
    DEBUG_PRINT("\tval: %08x\n", op.imm.rotated.value);
    result.value = rotateRight(result.value, op.imm.rotated.rotate * 2);
    DEBUG_PRINT("\trotate: %08x\n", op.imm.rotated.rotate);
    DEBUG_PRINT("\tresult: %08x\n", result.value);
  }
  else{//register value
    evaluateShiftedReg(state, op, &result);
  }
  return result;
}

shift_result_t evaluateOffset(state_t *state, flag_t I, operand_t op){
  shift_result_t result;
  if (I){
    evaluateShiftedReg(state, op, &result);
  }else{
    result.carry = 0;
    result.value = op.imm.fixed;
  }
  return result;
}

/**
* Executes the current decoded instruction
*
*/
void execute(state_t *state){
  instruction_t *decoded = state->pipeline.decoded;
  if(decoded->type == HAL){
    executeHAL(state);
    return;
  }
  DEBUG_PRINT("Flags: %04x:\n\t", getFlags(state));
  DEBUG_PRINT("Execute?: %01x:\n\t", condition(state, decoded->cond));
  if(condition(state, decoded->cond)){
    switch(decoded->type){
      case DP:
        DEBUG_PRINT("DP(%01x)\n\t\t", DP);
        executeDP(state, decoded->i.dp);
        break;
      case MUL:
        DEBUG_PRINT("MUL(%01x)\n\t\t", MUL);
        executeMUL(state, decoded->i.mul);
        break;
      case SDT:
        DEBUG_PRINT("SDT(%01x)\n\t\t", SDT);
        executeSDT(state, decoded->i.sdt);
        break;
      case BRN:
        DEBUG_PRINT("BRN(%01x)\n\t\t", BRN);
        executeBRN(state, decoded->i.brn);
        break;
      default:
        fprintf(stderr, "Invalid type%x\n", decoded->type);
        //free_state();
    }
  }
}

void executeDP(state_t *state, dp_instruction_t instr){
  shift_result_t barrel = evaluateOperand(state, instr.I, instr.operand2);
  word_t op2 = barrel.value;
  word_t result = 0;
  word_t rn = getRegister(state, instr.rn);
  switch(instr.opcode){
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
  DEBUG_PRINT("S: %01x\n", instr.S);
  DEBUG_PRINT("Result: %08x\n", result);
  if(instr.S){
    byte_t flags = 0x0;
    switch(instr.opcode){
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
        flags |= C * ((isNegative(rn) == isNegative(op2))
                      != isNegative(result));
        break;
    }
    flags |= (N * isNegative(result));
    flags |= (Z * (result == 0));
    DEBUG_PRINT("Flags were: %04x\n", getFlags(state));
    DEBUG_PRINT("Flags are: %04x\n", flags);
    setFlags(state, flags);
    DEBUG_PRINT("CPSR Updated: %04x\n", getFlags(state));
  }
  if(instr.opcode != TST && instr.opcode != TEQ && instr.opcode != CMP){
    setRegister(state, instr.rd, result);
  }
}

void executeMUL(state_t *state, mul_instruction_t instr) {
  // Cast the operands to 64 bit since this is the max result of A * B where A, B are 32 bit
  uint64_t Rm = getRegister(state, instr.rm);
  uint64_t Rs = getRegister(state, instr.rs);
  uint64_t Rn = getRegister(state, instr.rn);

  uint64_t res;
  if (instr.A) {
    res = Rm * Rs + Rn;
  } else {
    res = Rm * Rs;
  }

  // Mask to get only lower 32 bits of Rd, signed/unsigned does not affect these
  uint64_t mask = ~(UINT64_MAX - UINT32_MAX);
  uint32_t Rd = (u_int32_t) (res & mask);

  // Set flag bits
  if (instr.S) {
    byte_t flags = getFlags(state);

    // Set N flag
    byte_t N = (byte_t) getBits(Rd, 31, 31);
    flags |= (N << (uint8_t) 3);

    // Set Z flag
    byte_t Z = 0;
    if (Rd == 0) {
      Z = 1;
    }
    flags |= (Z << (uint8_t) 2);

    setFlags(state, flags);
  }

  // Store result in Rd
  setRegister(state, instr.rd, Rd);
}

/**
 *  Execute a branch instruction
 *
 * @param state - pointer to the program state
 * @param instr - the branch instruction to execute
 */
void executeBRN(state_t *state, brn_instruction_t instr){
  word_t pc = getPC(state);
  //Shift offset left by 2 bits
  word_t shiftedOffset =  lShiftLeft(instr.offset, 0x2);
  //Sign extend offset to 32 bits
  shiftedOffset |= (shiftedOffset >> 23) ? OFFSET_BITMASK : 0x0;
  //Assume that the offset takes into account the knowledge that the PC is
  // 8 bytes ahead of the instruction being executed.
  setPC(state, pc + (int32_t) shiftedOffset);
  // Fetch new word at PC
  state->pipeline.fetched = getMemWord(state, getPC(state));
}

void executeSDT(state_t *state, sdt_instruction_t instr){
  shift_result_t barrel = evaluateOffset(state, instr.I, instr.offset);
  word_t offset = barrel.value;
  word_t rn = getRegister(state, instr.rn);
  DEBUG_PRINT("Offset: 0x%08x\n\t\t", offset);
  DEBUG_PRINT("rn: 0x%08x\n\t\t", rn);
  DEBUG_PRINT("P = %01x\n\t\t", instr.P);
  if (instr.P){ //Pre-indexing

    if (instr.U){ //Add offset
      rn += offset;
    }else{  //Subtract offset
      rn -= offset;
    }
    DEBUG_PRINT("rn+-offset: 0x%08x\n\t\t", rn);
    if (instr.L){ //Load from memory at address rn into reg rd.
      DEBUG_PRINT("MEM[rn(%u)] -> R[rd(%u)]\n\t\t", rn, instr.rd);
      setRegister(state, instr.rd, getMemWord(state, rn));
      DEBUG_PRINT("r[rd(%u)] = 0x%08x\n", instr.rd, getMemWord(state, rn));
    }else{ //Store contents of reg rd in memory at address rn.
      DEBUG_PRINT("R[rd(%u)] -> MEM[rn(%u)]\n\t\t", instr.rd, rn);
      setMemWord(state, rn, getRegister(state, instr.rd));
      DEBUG_PRINT("MEM[%04x] = 0x%08x\n", rn, getRegister(state, instr.rd));
    }

  }else{ //Post-indexing
    if (instr.L){ //Load from memory at address rn into reg rd.
      DEBUG_PRINT("MEM[rn(%u)] -> rd(%u)\n\t\t", rn, instr.rd);
      setRegister(state, instr.rd, getMemWord(state, rn));
      DEBUG_PRINT("rd(%u) = 0x%08x\n", instr.rd, getMemWord(state, rn));
    }else{ //Store contents of reg rd in memory at address rn.
      DEBUG_PRINT("rd(%u) -> MEN[rn(%u)]\n\t\t", instr.rd, rn);
      setMemWord(state, rn, getRegister(state, instr.rd));
      DEBUG_PRINT("MEN[%04x] = rn(0x%08x)\n", getRegister(state, instr.rd), rn);
    }
    if (instr.U){ //Add offset
      rn += offset;
    }else{ //Subtract offset
      rn -= offset;
    }
    //Change contents of reg rn (the base register)
    DEBUG_PRINT("rn+-offset(0x%08x) -> rn(%04x)\n", rn, instr.rn);
    setRegister(state, instr.rn, rn);
  }
}

/**
 * Execute halt instruction
 *
 * @param state
 */
void executeHAL(state_t *state){
  printState(state);
}
