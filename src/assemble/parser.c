#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../assemble.h"
#include "parser.h"
#include "tokenizer.h"
#include "../utils/io.h"
#include "../utils/error.h"
#include "../utils/bitops.h"
#include "../utils/instructions.h"

#define PARSE_REG(Rn) (reg_address_t) atoi(remove_first_char(token_list_get_str(tklst, Rn)))
#define PARSE_EXPR(tok) ((int) strtol(remove_first_char(tok), NULL, 0))
#define COMPARE_OP(str) (strcmp(str, opcode) == 0)

const branch_suffix_to_cond brn_suffixes[NUM_BRN_SUFFIXES] = {
    {"eq", 0x0},
    {"ne", 0x1},
    {"ge", 0xA},
    {"lt", 0xB},
    {"gt", 0xC},
    {"le", 0xD},
    {"al", 0xE},
    {"",   0xE}
};

const opcode_to_parser oplist[NUM_NON_BRANCH_OPS] = {
    {"add", ADD, &parse_dp},
    {"sub", SUB, &parse_dp},
    {"rsb", RSB, &parse_dp},
    {"and", AND, &parse_dp},
    {"eor", EOR, &parse_dp},
    {"orr", ORR, &parse_dp},
    {"mov", MOV, &parse_dp},
    {"tst", TST, &parse_dp},
    {"teq", TEQ, &parse_dp},
    {"cmp", CMP, &parse_dp},

    {"mul", MOV, &parse_mul},   // Dummy value for opcode_t
    {"mla", MOV, &parse_mul},   // ...

    {"ldr", MOV, &parse_sdt},   // ...
    {"str", MOV, &parse_sdt},   // ...

    {"lsl", MOV,   &parse_lsl},   // ...
    {"andeq", MOV, &parse_halt} // ...
};

bool is_label(list_t *tklst) {
  return token_list_get_type(tklst, tklst->len - 1) == T_LABEL;
}

int str_to_enum(char *opcode){
  opcode_t op_enum;
  int isSet = 0;
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (COMPARE_OP(oplist[i].op)) {
      op_enum = oplist[i].op_enum;
      isSet = 1;
    }
  }
  if (!isSet) return -1;
  return op_enum;
}

char *remove_first_char(char *string) {
  return (string + 1);
}


/**
 *
 * @param : value
 * @return :
 */
op_rotate_immediate_t make_rotation(word_t value) {
  op_rotate_immediate_t op;
  byte_t rot = 0;

  while(get_bits(value, 31 , 8) != 0 && rot < MAX_ROT_VAL){
    value = rotate_left(value, 2);
    rot += 1;
  }

  if(rot == MAX_ROT_VAL){
    perror("Cannot convert value");
    exit(EC_INVALID_PARAM);
  }

  op.value = (byte_t) value;
  op.rotate = rot;
  return op;
}

/**
 * Get the immediate value from operand2
 *
 * @param operand2: a string containing the immediate value
 * @return the operand
 */
operand_t get_imm_op2(char *operand2) {
  DEBUG_CMD(printf("Immediate OP\n"));
  operand_t result;

  // Strip leading hash sign
  char *strVal = remove_first_char(operand2);

  DEBUG_PRINT("Operand2: %s\n", operand2);
  // Determine the base (hex or decimal) and convert to int
  uint8_t base = (uint8_t) ((strncmp("0x", strVal, 2) == 0) ? HEX_BASE : DEC_BASE);

  DEBUG_CMD(printf("Raw Val\n"));
  uint64_t raw_val = (uint64_t) strtoul(strVal, NULL, base);
  // Num cannot be represented if it is larger than 32 bits
  uint64_t big_mask = UINT64_MAX - UINT32_MAX;
  if ((raw_val & big_mask) != 0) {
    perror("number cannot be represented");
    exit(EXIT_FAILURE);
  }
  DEBUG_CMD(printf("Rotate\n"));
  // If num is bigger than 8 bits we must use a rotate
  uint32_t small_mask = UINT32_MAX - UINT8_MAX;
  if ((raw_val & small_mask) != 0) {
    result.imm.rotated = make_rotation((uint32_t) raw_val);
  } else {
    //imm.fixed is only used for sdt
    result.imm.rotated.value = (uint8_t) raw_val;
    result.imm.rotated.rotate = 0;
  }
  return result;
}

/*=============================================>>>>>
= DATA PROCESSING INSTRUCTION
===============================================>>>>>*/

/**
 * Parse a DP instruction
 *
 * @param :
 * @param :
 * @param :
 * @return :
 */

int parse_dp(assemble_state_t* prog, list_t *tklst, instruction_t *inst) {
  DEBUG_CMD(printf("----\nDP:\n"));

  // Get opcode enum
  char *opcode = token_list_get_str(tklst, 0);
  opcode_t op_enum = (opcode_t) str_to_enum(opcode);

  // Set whether the CPSR flags should be set
  bool S = COMPARE_OP("tst") || COMPARE_OP("teq") || COMPARE_OP("cmp");

  // Set position of rn and position of operand2
  int rn_pos = S || COMPARE_OP("mov") ? 1 : 3;

  // Set all instruction fields
  inst->type = DP;
  inst->i.dp.padding = 0x00;
  inst->i.dp.I = token_list_get_type(tklst, rn_pos + 2) == T_HASH_EXPR;
  inst->i.dp.opcode = op_enum;
  inst->i.dp.S = S;
  inst->i.dp.rn = COMPARE_OP("mov")? 0 : PARSE_REG(rn_pos);
  inst->i.dp.rd = S ? 0 : PARSE_REG(RD_POS);
  DEBUG_PRINT("RN_POS: %u\n", rn_pos);

  if(inst->i.dp.I) {
    inst->i.dp.operand2 = get_imm_op2(token_list_get_str(tklst, rn_pos + 2));
  } else {
    inst->i.dp.operand2 = (operand_t) {
      .reg.type = LSL, .reg.rm = PARSE_REG(rn_pos + 2), .reg.shiftBy = 0, .reg.shift.constant.integer = 0};
  }//should this be LSL instead of 0?

  return EC_OK;
}

/*= End of DATA PROCESSING INSTRUCTION =*/
/*=============================================<<<<<*/

/*=============================================>>>>>
= MULTIPLICATION INSTRUCTION
===============================================>>>>>*/

int parse_mul(assemble_state_t* prog, list_t *tklst, instruction_t *inst) {
  DEBUG_CMD(printf("MUL:\n"));
  flag_t A = (flag_t) strcmp(token_list_get_str(tklst, 0), "mul");

  reg_address_t rd = PARSE_REG(RD_POS);
  reg_address_t rm = PARSE_REG(RM_POS);
  reg_address_t rs = PARSE_REG(RS_POS);
  reg_address_t rn = A ? PARSE_REG(RN_POS) : 0;

  inst->type = MUL;
  inst->i.mul.pad0 = (byte_t) 0x0;
  inst->i.mul.A    = A;
  inst->i.mul.S    = 0;
  inst->i.mul.rd   = rd;
  inst->i.mul.rn   = rn;
  inst->i.mul.rs   = rs;
  inst->i.mul.pad9 = HEX_NINE;
  inst->i.mul.rm   = rm;

  return EC_OK;
}

/*= End of MULTIPLICATION INSTRUCTION =*/
/*=============================================<<<<<*/


/*=============================================>>>>>
= SINGLE DATA TRANSFER
===============================================>>>>>*/

/**
* Case 1: <code> Rd, <=expression>          {4}
* Case 2: <code> Rd , [ Rn ]                   {6}
* Case 3: <code> Rd, [Rn,<#expression>]     {8}
* Case 3b: <code> Rd, [Rn,{+/-}Rm{,<shift>}  {7-11}
* Case 4: <code> Rd, [Rn],<#expression>     {8}
* Case 4b: <code> Rd, [Rn],{+/-}Rm{,<shift>} {8-12}
*/

// TODO: check this works with loading GPIO addresses
int parse_sdt(assemble_state_t* prog, list_t *tklst, instruction_t *inst){
  DEBUG_CMD(printf("SDT:\n"));
  char *opcode = token_list_get_str(tklst, 0);
  flag_t L = COMPARE_OP("ldr");
  if(!L && !COMPARE_OP("str")){
    perror("Opcode not recognised\n");
  }
  inst->type = SDT;
  inst->i.sdt.L = L;
  inst->i.sdt.pad1 = 0x1;
  inst->i.sdt.pad0 = 0x0;
  inst->i.sdt.rd = PARSE_REG(RD_POS);

  int value = 0;
  // Case 1: =expr
  if(tklst->len == NUM_TOKS_EQ_EXPR){
    value = PARSE_EXPR(token_list_get_str(tklst, 3));

    if(value <= MAX_HEX){
      char * immVal = token_list_get_str(tklst, 3);
      immVal[0] = '#';
      list_t *mod_tklst = token_list_new();
      token_list_add_pair(mod_tklst, T_OPCODE, "mov");
      token_list_add(mod_tklst, token_list_get(tklst, 1));
      token_list_add(mod_tklst, token_list_get(tklst, 2));
      token_list_add_pair(mod_tklst, T_HASH_EXPR, immVal);

      return parse_dp(prog, mod_tklst, inst);
    } else {
      char *address_str = itoa(value); //TODO: Isn't meant to be storing the value,
                                      // mean't to be storing offset from current location to end of program_state data
      DEBUG_PRINT("address_st is: %s\n", address_str);
      int hash_expr_len = strlen(address_str) + 1;
      char *hash_expr = calloc(1, hash_expr_len);
      if(hash_expr == NULL){
        return EC_NULL_POINTER;
      }
      hash_expr[0] = '#';
      strcat(hash_expr, address_str);

      list_t *mod_tklst = token_list_new();
      token_list_add_pair(mod_tklst, T_OPCODE, "ldr");
      token_list_add(mod_tklst, token_list_get(tklst, 1));
      token_list_add(mod_tklst, token_list_get(tklst, 2));
      token_list_add_pair(mod_tklst, T_L_BRACKET, "[");
      token_list_add_pair(mod_tklst, T_REGISTER, "r15");
      token_list_add_pair(mod_tklst, T_COMMA, ",");
      token_list_add_pair(mod_tklst, T_HASH_EXPR, hash_expr);
      token_list_add_pair(mod_tklst, T_R_BRACKET, "]");

      DEBUG_PRINT("Modified hash_expr is: %s\n", hash_expr);
      int status = parse_sdt(prog, mod_tklst, inst);
      free(hash_expr);
      return status;
    }
  }
  // Case 2: [Rn]
  if(tklst->len == NUM_TOKS_PRE_IND_ADDR){
    inst->i.sdt.I = 0;
    inst->i.sdt.P = 1;
    inst->i.sdt.U = 1;
    inst->i.sdt.rn = PARSE_REG(4);
    inst->i.sdt.offset.imm.fixed = 0;
    return EC_OK;
  }
  if(tklst->len == NUM_TOKS_HASH_EXPR){
    DEBUG_PRINT("Hash Expression TOKENS: %d\n", NUM_TOKS_HASH_EXPR);
    inst->i.sdt.rn = PARSE_REG(4);
    inst->i.sdt.U = 1;
    inst->i.sdt.I = 0;
    // Case 3: [Rn, #expression]
    if(token_list_get_type(tklst, 5) == T_COMMA){
      inst->i.sdt.P = 1;
      inst->i.sdt.offset.imm.fixed = (word_t) PARSE_EXPR(token_list_get_str(tklst, 6));
      return EC_OK;
    }

    // Case 4: [Rn],<#expression>
    if(token_list_get_type(tklst, 5) == T_R_BRACKET){
      inst->i.sdt.P = 0;
      inst->i.sdt.offset.imm.fixed = (word_t) PARSE_EXPR(token_list_get_str(tklst, 7));
      return EC_OK;
    }
  }
  return EC_UNSUPPORTED_OP;
}

/*= End of SINGLE DATA TRANSFER =*/
/*=============================================<<<<<*/

/*=============================================>>>>>
= BRANCH INSTRUCTION
===============================================>>>>>*/


/**
* Update memory according to reference entry
*
* @param label : current string representation of the label in entry
* @param val : current value of entry
* @param obj : A prog_collection_t object which collects, program, label and addr.
*/
void ref_entry(const label_t label, const address_t val, const void *obj){
  prog_collection_t *prog_coll = (prog_collection_t *) obj;
  if(label == prog_coll->label){
    prog_coll->prog->out[val] = prog_coll->addr;
  }
}

/**
 * Add a symbol to the symbol table in the program_state and update reference table
 *
 * @param program_state : pointer to the program_state information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int add_symbol(assemble_state_t *program, label_t label, address_t addr) {
  if (!smap_put(program->smap, label, addr)) {
    return 0; // already in symbol map
  }
  prog_collection_t prog_coll = {program, label, addr};
  // check if symbol exists in rmapap and update/remove accordingly
  if (rmap_exists(program->rmap, label)) {
    rmap_enum(program->rmap, ref_entry, &prog_coll);
  }

  return 1;
}

/**
 * add a symbol to the reference map stored in the program
 *
 * @param program_state : pointer to the program_state information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int add_reference(assemble_state_t *program, label_t label, address_t addr) {
  // adds reference to rmapap.
  return !rmap_put(program->rmap, label, addr);
}

/**
 * Calculate the offset of an address from the current program_state Counter, to be stored in a BRN instruction
 *
 * @param address: the address to calculate the offset of
 * @param PC: the address of the branch instruction being executed
 * @return: the offset to be store - a shifted 24 bit value
 */
word_t calculate_offset(int address, word_t PC) {
  word_t offset = address - PC - 8;

  // Check we can store in 26 bits
  if (get_bits(offset, 31, 25) != 0) {
    perror("Offset was too large to store");
    return 1;
  }

  // Shift right by 2 then store (in 24 bits)
  shift_result_t shifted_offset = a_shift_right_c(offset, 2);
  if (shifted_offset.carry == true) {
    perror("carry occurred when trying to store offset");
    return 1;
  }
  return shifted_offset.value;
}

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
int parse_brn(assemble_state_t* prog, list_t *tklst, instruction_t *inst) {
  // Parse for condition
  char *suffix = remove_first_char(token_list_get_str(tklst, 0));
  for (int i = 0; i < NUM_BRN_SUFFIXES; i++) {
    if (strcmp(brn_suffixes[i].suffix, suffix) == 0) {
      inst->cond = brn_suffixes[i].cond;
    }
  }

  word_t offset;
  if (token_list_get_type(tklst, 1) == T_STR) {
    // TODO
    // Check if label is already in map, if so get address
    char *label = token_list_get_str(tklst, 1);
    if (smap_exists(prog->smap, label)) {
      DEBUG_PRINT("Getting label: (%s @%08x)\n", label, prog->mPC);
      address_t addr = 0;
      smap_get_address(prog->smap, label, &addr);
      DEBUG_PRINT("Got Address: %08x\n", addr);
      offset = calculate_offset(addr, prog->mPC);
    } else {
      DEBUG_PRINT("Placing (%s, %08x) in reference\n", label, prog->mPC);
      rmap_put(prog->rmap, label, prog->mPC);
      offset = 0xFFFFFF; // dummy value
    }
  } else {
    offset = calculate_offset(atoi(token_list_get_str(tklst, 0)), prog->mPC);
  }

  inst->type = BRN;
  inst->i.brn.padA = 0xA;//15u & (unsigned) HEX_TEN; //this is necessary to remove gcc warning
  inst->i.brn.offset = offset;

  return EC_OK;
}

/*=============================================>>>>>
= SPECIAL INSTRUCTIONS
===============================================>>>>>*/


int parse_lsl(assemble_state_t* prog, list_t *tklst, instruction_t *inst) {
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

  return EC_OK;
}

// TODO: use this if we support mov with a shifted register in DP
int parse_lsl_conversion(assemble_state_t *prog, list_t *tklst, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  // Create new instruction in memory, to be parsed by sdt
  list_t *mod_tklst = token_list_new();
  token_list_add_pair(mod_tklst, T_OPCODE, "mov");
  token_list_add(mod_tklst, token_list_get(tklst, 1));
  token_list_add(mod_tklst, token_list_get(tklst, 2));
  token_list_add(mod_tklst, token_list_get(tklst, 1));
  token_list_add(mod_tklst, token_list_get(tklst, 2));
  token_list_add_pair(mod_tklst, T_SHIFT, "lsl");
  token_list_add(mod_tklst, token_list_get(tklst, 3));

  return parse_sdt(prog, mod_tklst, inst);
}

int parse_halt(assemble_state_t *prog, list_t *tklst, instruction_t *inst) {
  DEBUG_CMD(printf("HAL:\n"));
  inst->type = HAL;
  inst->i.hal.pad0 = 0u;
  return EC_OK;
}

/*=============================================>>>>>
= LABEL INSTRUCTIONS
===============================================>>>>>*/

int parse_label(assemble_state_t *prog, list_t *tklst) {
  int _status = EC_OK;
  char *label = token_list_get_str(tklst, 0);
  if(smap_exists(prog->smap, label)){
    return EC_SKIP;
  }
  smap_put(prog->smap, label, prog->mPC);
  DEBUG_CMD(rmap_print(prog->rmap));
  if(rmap_exists(prog->rmap, label)){
    int num_references = rmap_get_references(prog->rmap, label, NULL, 0);
    size_t size_ref = num_references * sizeof(address_t);
    address_t *addrs = malloc(size_ref);
    if(addrs == NULL){
      perror("parse_label(): malloc failed");
      return EC_NULL_POINTER;
    }
    if((_status = rmap_get_references(prog->rmap, label, addrs, size_ref))){
      return _status;
    }
    for (int i = 0; i < num_references; i++) {
      word_t offset = calculate_offset(prog->mPC, addrs[i]) | 0xFF000000;
      word_t curr;
      get_word(prog->out, addrs[i], &curr);
      curr &= offset;
      set_word(prog->out, addrs[i], curr);
      DEBUG_PRINT("REFERENCE: %u\n", addrs[i]);
    }
  }
  return _status;
}

/**
 *  Translates a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form by calling the appropriate
 *  sub-functions
 *
 *  @param tokens: a pointer to the array of tokens
 *  @param inst: a pointer to the instruction to be stored
 */
int parse(assemble_state_t *prog, list_t *tklst, instruction_t *inst) {
  // If the assembly line is a label
  if (is_label(tklst)) {
    DEBUG_CMD(printf("LABEL:\n"));
    parse_label(prog, tklst);
    return EC_SKIP;
  }

  // Get the pointer to the first token - this will be the opcode
  char *opcode = token_list_get_str(tklst, 0);

  // Parse a branch instruction and its condition
  if (!strncmp(opcode, "b", 1)) {
    DEBUG_CMD(printf("BRANCH:\n"));
    return parse_brn(prog, tklst, inst);
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = AL_COND_CODE; //0b1110

  if (!strcmp("lsl", opcode)) {
    DEBUG_CMD(printf("LSL:\n"));
    perror("lsl not supported yet");
    return EC_UNSUPPORTED_OP;
  }

  // Calculate function pointer to parse an instruction from the opcode
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      return oplist[i].parse_func(prog, tklst, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return EC_UNSUPPORTED_OP;
}
