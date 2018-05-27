#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "bitops.h"
#include "register.h"
#include "io.h"

/**
* Checks if the condition on the decoded instruction is met using the current
* state of the flags register (CPSR).
*
* @param state: The current VM state
* @param cond: The condition extracted from the instructionn
* @returns 1 when condition is met, 0 if not.
*/
int condition(state_t *state, byte_t cond){
  byte_t flags = getNibble(state->registers.cpsr, sizeof(word_t));

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

shift_result_t evaluateOperand(state_t *state, flag_t I, operand_t op){
  shift_result_t result;
  if(I){ //Immediate value
    result.value = leftPadZeros(op.imm.value);
    result.value = rotateRight(result.value, op.imm.rotate);
  }
  else{//register value
    word_t rm = getRegister(state, op.reg.rm);
    byte_t shiftAmount = 0;
    if(op.reg.shiftBy){ //Shift by register
      shiftAmount = getByte(getRegister(state, op.reg.shift.shiftreg.rs), 7);
    }
    else{ //Shift by constnat
      shiftAmount = op.reg.shift.constant.integer;
    }
    switch(op.reg.type){
      case LSL:
        result = lShiftLeftC(rm, shiftAmount);
        break;
      case LSR:
        result = lShiftRightC(rm, shiftAmount);
        break;
      case ASR:
        result = aShiftRightC(rm, shiftAmount);
        break;
      case ROR:
        result = rotateRightC(rm, shiftAmount);
        break;
      default:
        //TODO: Add proper error handling code
        exit(EXIT_FAILURE);
    }
  }
  return result;
}
/**
*
*/
void execute(state_t *state){
  instruction_t *decoded = state->pipeline.decoded;
  if(decoded->type == HAL){
    executeHAL(state);
    return;
  }
  if(condition(state, decoded->cond)){
    switch(decoded->type){
      case DP:
        executeDP(state, decoded->i.dp);
        break;
      case MUL:
        executeMUL(state, decoded->i.mul);
        break;
      case SDT:
        executeSDT(state, decoded->i.sdt);
        break;
      case BRN:
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
    setFlags(state, flags);
  }
  if(instr.opcode != TST && instr.opcode != TEQ && instr.opcode != CMP){
    setRegister(state, instr.rd, result);
  }
}
void executeMUL(state_t *state, mul_instruction_t instr){
  return;
}
void executeBRN(state_t *state, brn_instruction_t instr){
  return;
}
void executeSDT(state_t *state, sdt_instruction_t instr){
  return;
}
void executeHAL(state_t *state){
  printState(state);
}
