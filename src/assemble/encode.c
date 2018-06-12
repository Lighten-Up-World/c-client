#include <assert.h>
#include <stdio.h>

#include "../utils/error.h"
#include "../utils/arm.h"
#include "../utils/bitops.h"
#include "../utils/register.h"
#include "encode.h"
/**
 * encode shifted register for DP or SDT instruction
 *
 * @param instr
 * @param word
 * @return
 */

int encode_shifted_reg(op_shiftreg_t opShiftReg, word_t *w){
  assert(w != NULL);
  DEBUG_PRINT("ENCODE_SHIFTED_REG: %08x\n", *w);
  if (!is_valid_register(opShiftReg.rm)){
    return EC_INVALID_PARAM;
  }
  if (opShiftReg.shiftBy){ //Shift by reg
    if (!is_valid_register(opShiftReg.shift.shiftreg.rs)){
      return EC_INVALID_PARAM;
    }
    *w <<= REG_SIZE;
    *w |= opShiftReg.shift.shiftreg.rs;
    *w <<=1;
  }else{
    *w <<=OP_SHIFT_INT_SIZE;
    *w |= opShiftReg.shift.constant.integer;
  }
  *w <<=OP_SHIFT_TYPE_SIZE;
  *w |= opShiftReg.type;
  *w <<= FLAG_SIZE;
  *w |= opShiftReg.shiftBy;
  *w <<= REG_SIZE;
  *w |= opShiftReg.rm;

  return EC_OK;
}


/**
 * encode the operand for the dp instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_operand(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);
  DEBUG_PRINT("encode_operand: %08x\n", *w);
  if (instr->i.dp.I){ //Operand2 is immediate constant
    *w <<= OP_ROTATE_SIZE;
    *w |= instr->i.dp.operand2.imm.rotated.rotate;
    *w <<= OP_IMM_SIZE;
    *w |= instr->i.dp.operand2.imm.rotated.value;
    return EC_OK;
  }else{  //Operand2 is shifted register
    return encode_shifted_reg(instr->i.dp.operand2.reg, w);
  };
}

/**
 * encode the offset for the sdt instruction
 *
 * @param instr
 * @param word
 * @return
 */

int encode_offset(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);
  DEBUG_PRINT("encode_offset: %08x\n", *w);
  if (instr->i.dp.I){ //Offset is shifted register
    return encode_shifted_reg(instr->i.sdt.offset.reg, w);
  }else{  //Offset is 12-bit immediate value
    *w <<= SDT_OFFSET_SIZE;
    *w |= instr->i.sdt.offset.imm.fixed;
    return EC_OK;
  }
}



/**
 * encode for the dp instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_dp(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);

  *w <<= (DP_PAD0_SIZE + FLAG_SIZE);
  *w |= instr->i.dp.I;
  switch (instr->i.dp.opcode){
    case AND:
    case EOR:
    case SUB:
    case RSB:
    case ADD:
    case TST:
    case TEQ:
    case CMP:
    case ORR:
    case MOV:
      *w <<= OPCODE_SIZE;
      *w |= instr->i.dp.opcode;
      break;
    default:
      return EC_INVALID_PARAM;
  }
  *w <<= FLAG_SIZE;
  *w |= instr->i.dp.S;

  if (instr->i.dp.rn >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.dp.rn;

  if (instr->i.dp.rd >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.dp.rd;

  if (encode_operand(instr, w)){
    return EC_INVALID_PARAM;
  }

  return EC_OK;
}

/**
 * encode the mul instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_mul(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);

  *w <<= (MUL_PAD0_SIZE + FLAG_SIZE);
  *w |= instr->i.mul.A;
  *w <<= FLAG_SIZE;
  *w |= instr->i.mul.S;

  if (instr->i.mul.rd >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.mul.rd;

  if (instr->i.mul.rn >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.mul.rn;

  if (instr->i.mul.rs >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.mul.rs;

  *w <<= PAD9_SIZE;
  *w |= instr->i.mul.pad9;

  if (instr->i.mul.rs >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.mul.rm;

  return EC_OK;
}

/**
 * encode the sdt instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_sdt(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);

  DEBUG_PRINT("SDT: %x|%x|%x|%x|%x|%x|%x|%x|%x|%x|\n",
instr->cond,
instr->i.sdt.pad1,
instr->i.sdt.I,
instr->i.sdt.P,
instr->i.sdt.U,
instr->i.sdt.pad0,
instr->i.sdt.L,
instr->i.sdt.rn,
instr->i.sdt.rd,
instr->i.sdt.offset.imm.fixed);

  *w <<= SDT_PAD1_SIZE;
  *w |= instr->i.sdt.pad1;

  *w <<= FLAG_SIZE;
  *w |= instr->i.sdt.I;

  *w <<= FLAG_SIZE;
  *w |= instr->i.sdt.P;

  *w <<= FLAG_SIZE;
  *w |= instr->i.sdt.U;

  *w <<= (SDT_PAD0_SIZE + FLAG_SIZE);
  *w |= instr->i.sdt.L;

  if (!is_valid_register(instr->i.sdt.rn)){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.sdt.rn;

  if (!is_valid_register(instr->i.sdt.rd)){
    return EC_INVALID_PARAM;
  }
  *w <<= REG_SIZE;
  *w |= instr->i.sdt.rd;

  if(encode_offset(instr, w)){
    return EC_INVALID_PARAM;
  }

  return EC_OK;
}

/**
 * encode the brn instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_brn(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);

  *w <<= BRN_PADA_SIZE;
  *w |= instr->i.brn.padA;
  *w <<= BRN_OFFSET_SIZE;
  *w |= instr->i.brn.offset;

  return EC_OK;
}

/**
 * encode the hal instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_hal(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);
  if (instr->i.hal.pad0){
    return EC_INVALID_PARAM;
  }
  return EC_OK;
}


/**
 * encode cond
 *
 * @param instr - pointer to instruction to be encoded into binary
 * @param word - pointer to encoded binary word
 * @return status code for success of encoding
 */

int encode_cond(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);
  switch (instr->cond){
    case EQ:
    case NE:
    case GE:
    case LT:
    case GT:
    case LE:
    case AL:
      *w = instr->cond;
      return EC_OK;
    default:
      return EC_INVALID_PARAM;
  }
}

/**
 * encode entry function
 *
 * @param instr - pointer to instruction to be encoded into binary
 * @param word - pointer to encoded binary word
 * @return status code for success of encoding
 */
int encode(instruction_t *instr, word_t *w){
  assert(w != NULL);
  assert(instr != NULL);
  *w = 0;
  if(encode_cond(instr, w)){
    return EC_INVALID_PARAM;
  }
  switch (instr->type){
    case DP:
      return encode_dp(instr, w);
    case MUL:
      return encode_mul(instr, w);
    case SDT:
      return encode_sdt(instr, w);
    case BRN:
      return encode_brn(instr, w);
    case HAL:
      return encode_hal(instr, w);
    default:
      return EC_INVALID_PARAM;
  }
}
