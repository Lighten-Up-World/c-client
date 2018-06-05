#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils/instructions.h>
#include "parser.h"
#include "tokenizer.h"
#include "../utils/instructions.h"

#define PARSE_REG(Rn) (reg_address_t) atoi(remove_first_char(tokens[Rn].str))
#define COMPARE_OP(str) (strcmp(str, opcode) == 0)

int str_to_enum(char *opcode){
  opcode_t op_enum = NULL;
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (COMPARE_OP(oplist[i].op)) {
      op_enum = oplist[i].op_enum;
    }
  }
  if (op_enum == NULL) return -1;
  return op_enum;
}

char *remove_first_char(char *string) {
  return (string + 1);
}

/**
 *
 * @param value
 * @return
 */
op_rotate_immediate_t make_rotation(uint32_t value) {
  op_rotate_immediate_t op;









  return op;
}

//TODO: rotated values
/**
 * Get the immediate value from operand2
 *
 * @param operand2: a string containing the immediate value
 * @return the operand
 */
operand_t get_op2(char *operand2) {
  operand_t result;

  // Strip leading equals sign
  char *strVal = remove_first_char(operand2);

  // Determine the base (hex or decimal) and convert to int
  uint8_t base = (uint8_t) ((strncmp("0x", operand2, 2) == 0) ? 16 : 10);
  if (base == 16) {
    strVal = remove_first_char(remove_first_char(strVal));
  }
  uint64_t raw_val = (uint64_t) strtoul(strVal, NULL, base);

  // Num cannot be represented if it is larger than 32 bits
  uint64_t big_mask = UINT64_MAX - UINT32_MAX;
  if ((raw_val & big_mask) != 0) {
    perror("number cannot be represented");
  }

  // If num is bigger than 8 bits we must use a rotate
  uint32_t small_mask = UINT32_MAX - UINT8_MAX;
  if ((raw_val & small_mask) != 0) {
    result.imm.rotated = make_rotation((uint32_t) raw_val);
  } else {
    result.imm.fixed = (uint8_t) raw_val;
  }

  return result;
}

/*=============================================>>>>>
= DATA PROCESSING INSTRUCTION
===============================================>>>>>*/

int parse_dp(program_t* prog, token_t *tokens, instruction_t *inst) {
  // Get opcode enum
  char *opcode = tokens[0].str;
  opcode_t op_enum = str_to_enum(opcode);
  // Set whether the CPSR flags should be set
  bool S = COMPARE_OP("tst") || COMPARE_OP("teq") || COMPARE_OP("cmp");
  // Set position of rn and position of operand2
  int rn_pos = S ? 1 : 2;

  // Set all instruction fields
  inst->type = DP;
  inst->i.dp.padding = 0x00;
  inst->i.dp.I = 1;
  inst->i.dp.opcode = op_enum;
  inst->i.dp.S = S;
  inst->i.dp.rn = PARSE_REG(rn_pos);
  inst->i.dp.rd = PARSE_REG(1);
  inst->i.dp.operand2 = get_op2(tokens[rn_pos + 1].str);

  return EC_OK;
}

/*= End of DATA PROCESSING INSTRUCTION =*/
/*=============================================<<<<<*/

int parse_mul(program_t* prog, token_t *tokens, instruction_t *inst) {
  flag_t A = strcmp(tokens[0].str, "mul");

  reg_address_t rd = PARSE_REG(1);
  reg_address_t rn = PARSE_REG(2);
  reg_address_t rs = PARSE_REG(3);
  reg_address_t rm = A ? PARSE_REG(0) : 0;

  inst->type = MUL;
  inst->i.mul.pad0 = (byte_t) 0x0;
  inst->i.mul.A    = A;
  inst->i.mul.S    = false;
  inst->i.mul.rd   = rd;
  inst->i.mul.rn   = rn;
  inst->i.mul.rs   = rs;
  inst->i.mul.pad9 = (byte_t) 0x1001;
  inst->i.mul.rm   = rm;

  return EC_OK;
}

//TODO: calculate offset and corresponding flags

/*=============================================>>>>>
= SINGLE DATA TRANSFER
===============================================>>>>>*/

/**
* Starts at 
*/
int parse_sdt_address(program_t* prog, token_t *tokens, instruction_t *inst){

  // Immediate value
  if (tokens[2].str[0] == '=') {
    I = 0;
    if (offset <= 0xFF) {
      //convert to mov
      return 0;
    }
    // ...
    // needs to be set in here, since structure of unions depends on imm...
    inst->i.sdt.offset.imm.fixed = offset;
  }
}

/**
* Parses a Single Data Transfer instruction
* Syntax is: <code> Rd, <address>
* <code>    - ldr|str
* <address> - <=expression>|[Rn]|[Rn, <#expression>]|[Rn,{+/-}Rm{,<shift>}]
              |[Rn],<#expression>| [Rn],{+/-}Rm{,<shift>}
*/
int parse_sdt(program_t* prog, token_t *tokens, instruction_t *inst) {
  char *opcode = tokens[0].str;
  flag_t L = COMPARE_OP("ldr");
  if(!L && !COMPARE_OP("str")){
    perror("Opcode not recognised\n");
  }

  // Offset is one of: constant, pre-index address, post-indexed address
  flag_t I; //immediate
  flag_t P; //post/pre-indexing
  flag_t U; //up bit (not supported, optional for +/-)
  reg_address_t rn; //base register address
  word_t offset;


  inst->type = SDT;
  inst->i.sdt.pad0 = 0x01;
  inst->i.sdt.I = I;
  inst->i.sdt.P = P;
  inst->i.sdt.U = U;
  inst->i.sdt.pad0 = 0x0;
  inst->i.sdt.L = L;
  inst->i.sdt.rn = rn;
  inst->i.sdt.rd = PARSE_REG(1);

  return 0;
}

/*= End of SINGLE DATA TRANSFER =*/
/*=============================================<<<<<*/

// TODO: calculate offset
/**
* Parses a branch instruction
* Syntax is: b<code> <expression>
* <code>       - eq|ne|ge|lt|gt|al|
* <expression> - Label name whose address is retreived
*
* The <expression> is the target address, which may be a label.
* The assembler should compute the offset between the current address and
* the label, taking into account the off-by-8 bytes effect that will occur
* due to the ARM pipeline. This signed offset should be 26 bits in length,
* before being shifted right two bits and having the lower 24 bits stored
* in the Offset field
*/
int parse_brn(program_t* prog, token_t *tokens, instruction_t *inst) {
  // Parse for condition
  char *suffix = remove_first_char(tokens[0].str);
  for (int i = 0; i < NUM_BRN_SUFFIXES; i++) {
    if (strcmp(brn_suffixes[i].suffix, suffix) == 0) {
      inst->cond = brn_suffixes[i].cond;
    }
  }

  // Parse for offset
  word_t offset;

  // Check if offset is a label
  if (offset_is_label(offset)) {
    // do something
  } else {
    // need to calculate offset for address given?
    // what's our current address?
  }
  inst->type = BRN;
  inst->i.brn.padA = (byte_t) 0x1010;
  inst->i.brn.offset = offset;

  return EC_OK;
}

int parse_lsl(program_t* prog, token_t *tokens, instruction_t *inst) {
  //Treat lsl Rn, <expr> as mov Rn, Rn, lsl <expr>
  reg_address_t r = PARSE_REG(1);

  inst->i.dp.padding = 0x00;
  inst->i.dp.I = 0;
  inst->i.dp.opcode = MOV;
  inst->i.dp.S = 1;
  inst->i.dp.rn = r;
  inst->i.dp.rd = r;
  //TODO: set this properly
  //inst->i.dp.operand2.reg.shift.constant.integer = get_op2(tokens[2].str).imm.fixed;
  inst->i.dp.operand2.reg.type = LSL;
  inst->i.dp.operand2.reg.shiftBy = 0; //case where this (bit 4) is 1 is optional
  inst->i.dp.operand2.reg.rm = r;
}

// TODO: use this if we support mov with a shifted register in DP
int parse_lsl_conversion(token_t *tokens, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  // Create new instruction in memory, to be parsed by sdt
  token_t mod_tokens[] = {
    {T_OPCODE, "mov"},
    tokens[1],
    tokens[2],
    tokens[1],
    tokens[2],
    {T_SHIFT, "lsl"},
    tokens[3]
  };



  return parse_sdt(&mod_tokens, inst);
}

int parse_halt(token_t *tokens, instruction_t *inst) {
  inst->type = HAL;
  inst->i.hal.pad0 = 0u;
  return EC_OK;
}

bool is_label(token_t *tokens, int tkn) {
  return tokens[tkn - 1].type == T_LABEL;
}

void parse_label() {}

/**
 *  Translates a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form by calling the appropriate
 *  sub-functions
 *
 *  @param tokens: a pointer to the array of tokens
 *  @param inst: a pointer to the instruction to be stored
 */
int parse(program_t *prog, token_t *tokens, instruction_t *inst, int tkn) {
  // If the assembly line is a label
  if (is_label(tokens, tkn)) {
    parse_label();
    return 0;
  }

  // Get the pointer to the first token - this will be the opcode
  char *opcode = tokens[0].str;

  // Parse a branch instruction and its condition
  if (strncmp(opcode, "b", 1) == 0) {
    return parse_brn(prog, tokens, inst);
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = 0xE; //0b1110

  // Calculate function pointer to parse an instruction from the opcode
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      return oplist[i].parse_func(prog, tokens, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return EC_UNSUPPORTED_OP;
}
