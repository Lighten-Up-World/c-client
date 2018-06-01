#include <instructions.h>
#include <assert.h>
#include <error.h>
#include <bitops.h>
#include "encode.h"

/**
 * encode shifted register for DP or SDT instruction
 *
 * @param instr
 * @param word
 * @return
 */

int encode_shifted_reg(op_shiftreg_t opShiftReg, word_t *word){
  if (opShiftReg.rm >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  word_t w = *word;
  if (opShiftReg.shiftBy){ //Shift by reg
    if (opShiftReg.shift.shiftreg.rs >= NUM_GENERAL_REGISTERS){
      return EC_INVALID_PARAM;
    }
    w <<= 4;
    w |= opShiftReg.shift.shiftreg.rs;
    w <<=1;
  }else{
    w <<=5;
    w |= opShiftReg.shift.constant.integer;
  }
  w <<=2;
  w |= opShiftReg.type;
  w <<=1;
  w |= opShiftReg.shiftBy;
  *word = w;
  return EC_OK;
}


/**
 * encode the operand for the dp instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_operand(instruction_t *instr, word_t *word){
  word_t w = *word;
  if (instr->i.dp.I){ //Operand2 is immediate constant
    w <<= 4;
    w |= instr->i.dp.operand2.imm.rotated.rotate;
    w <<= 8;
    w |= instr->i.dp.operand2.imm.rotated.value;
    *word = w;
    return EC_OK;
  }else{  //Operand2 is shifted register
    return encode_shifted_reg(instr->i.dp.operand2, word);
  };
}

/**
 * encode the offset for the sdt instruction
 *
 * @param instr
 * @param word
 * @return
 */

int encode_offset(instruction_t *instr, word_t *word){
  if (instr->i.dp.I){ //Offset is shifted register
    return encode_shifted_reg(instr->i.sdt.offset, word);
  }else{  //Offset is 12-bit immediate value

  }
  return 0;
}



/**
 * encode for the dp instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_dp(instruction_t *instr, word_t *word){

  word_t w = *word;
  w <<= 3;
  w |= instr->i.dp.I;
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
      w <<= 4;
      w |= instr->i.dp.opcode;
      break;
    default:
      return EC_INVALID_PARAM;
  }
  w <<= 1;
  w |= instr->i.dp.S;
  if (instr->i.dp.rn >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  w <<= 4;
  w |= instr->i.dp.rn;
  if (instr->i.dp.rd >= NUM_GENERAL_REGISTERS){
    return EC_INVALID_PARAM;
  }
  w <<= 4;
  w |= instr->i.dp.rd;
  if (encode_operand(instr, word)){
    return EC_INVALID_PARAM;
  }
  *word = w;
  return EC_OK;
}

/**
 * encode the mul instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_mul(instruction_t *instr, word_t *word){
  //TODO
  return 0;
}

/**
 * encode the sdt instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_sdt(instruction_t *instr, word_t *word){
  //TODO
  return 0;
}

/**
 * encode the brn instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_brn(instruction_t *instr, word_t *word){
  //TODO
  return 0;
}

/**
 * encode the hal instruction
 *
 * @param instr
 * @param word
 * @return
 */
int encode_hal(instruction_t *instr, word_t *word){
  //TODO
  return 0;
}


/**
 * encode cond
 *
 * @param instr - pointer to instruction to be encoded into binary
 * @param word - pointer to encoded binary word
 * @return status code for success of encoding
 */

int encode_cond(instruction_t *instr, word_t *word){
  word_t w = *word;
  switch (instr->cond){
    case EQ:
    case NE:
    case GE:
    case LT:
    case GT:
    case LE:
    case AL:
      w <<= 4;
      w |= instr->cond;
      *word = w;
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
int encode(instruction_t *instr, word_t *word){
  *word = 0;
  if(encode_cond(instr, word)){
    return EC_INVALID_PARAM;
  }
  switch (instr->type){
    case DP:
      return encode_dp(instr, word);
    case MUL:
      return encode_mul(instr, word);
    case SDT:
      return encode_sdt(instr, word);
    case BRN:
      return encode_brn(instr, word);
    case HAL:
      return encode_hal(instr, word);
    default:
      assert(false);
      return EC_INVALID_PARAM;
  }
}
