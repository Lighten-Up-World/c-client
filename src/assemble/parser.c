#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <instructions.h>
#include "../assemble.h"
#include "parser.h"
#include "tokenizer.h"
#include "../utils/error.h"
#include "../utils/bitops.h"
#include "../utils/instructions.h"

#define PARSE_REG(Rn) (reg_address_t) atoi(remove_first_char(tlst->tkns[Rn].str))
#define PARSE_EXPR(tok) ((int) strtol((tok)+1, NULL, 0))
#define COMPARE_OP(str) (strcmp(str, opcode) == 0)
#define GET_TKN(i) tlst->tkns[i]
#define GET_STR(i) tlst->tkns[i].str
#define GET_TYPE(i) tlst->tkns[i].type

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

int str_to_enum(char *opcode){
  opcode_t op_enum = -1;
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (COMPARE_OP(oplist[i].op)) {
      op_enum = oplist[i].op_enum;
    }
  }
  if (op_enum == -1) return -1;
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

  while(get_bits(value, 31 , 8) != 0 && rot < 16){
    value = rotate_left(value, 2);
    rot += 1;
  }
  if(rot == 16){
    perror("Cannot convert value");
  }
  //TODO: Understand this line
  //word_t imm = (rot << 8) | value;
  op.value = (byte_t) value;
  op.rotate = rot;
  return op;
}

//TODO: rotated values
/**
 * Get the immediate value from operand2
 *
 * @param operand2: a string containing the immediate value
 * @return the operand
 */
operand_t get_imm_op2(char *operand2) {
  operand_t result;

  // Strip leading hash sign
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
int parse_dp(program_t* prog, token_list_t *tlst, instruction_t *inst) {
  // Get opcode enum
  char *opcode = GET_STR(0);
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
  inst->i.dp.operand2 = get_imm_op2(GET_STR(rn_pos + 1));

  return EC_OK;
}

/*= End of DATA PROCESSING INSTRUCTION =*/
/*=============================================<<<<<*/

/*=============================================>>>>>
= MULTIPLICATION INSTRUCTION
===============================================>>>>>*/

int parse_mul(program_t* prog, token_list_t *tlst, instruction_t *inst) {

  flag_t A = (flag_t) strcmp(GET_STR(0), "mul");

  reg_address_t rd = PARSE_REG(1);
  reg_address_t rm = PARSE_REG(2);
  reg_address_t rs = PARSE_REG(3);
  reg_address_t rn = A ? PARSE_REG(4) : 0;

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

/*= End of MULTIPLICATION INSTRUCTION =*/
/*=============================================<<<<<*/

//TODO: calculate offset and corresponding flags

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
int parse_sdt_address(program_t* prog, token_list_t *tlst, instruction_t *inst){
  int address = 0;
  // Case 1: =expr
  if (tlst->n == 4) {
    address = PARSE_EXPR(GET_TKN(4));

    if(address <= 0xFF){
      token_t mod_tkns[] = {
        {T_OPCODE, "mov"},
        GET_TKN(1),
        GET_TKN(2)
        //TODO:
      };
      token_list_t mod_tlst = {mod_tkns, 7};

      return parse_dp(prog, &mod_tlst, inst);
    }

    int offset = address - prog->mPC - 8;

    char *newline = NULL;
    asprintf(&newline, "ldr %s, [PC, #%d]", GET_STR(1), offset);
    tokenize(newline, tlst);
    return parse_sdt(prog, tlst, inst);
  }

  // Case 2: [Rn]
  if(tlst->n == 6){

  }

  if(tlst->n == 8){
    // Case 3: [Rn, #expression]
    if(GET_TYPE(6) == T_COMMA){

    }
    // Case 4: [Rn],<#expression>
    if(GET_TYPE(6) == T_R_BRACKET){

    }
  }
  return EC_UNSUPPORTED_OP;
}

/**
* Parses a Single Data Transfer instruction
* Syntax is: <code> Rd, <address>
* <code>    - ldr|str
* <address> - <=expression>|[Rn]|[Rn, <#expression>]|[Rn,{+/-}Rm{,<shift>}]
              |[Rn],<#expression>| [Rn],{+/-}Rm{,<shift>}
*/
int parse_sdt(program_t* prog, token_list_t *tlst, instruction_t *inst) {
  char *opcode = GET_STR(0);
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
  //TODO:
  return 0;
}

/*= End of SINGLE DATA TRANSFER =*/
/*=============================================<<<<<*/

/*=============================================>>>>>
= BRANCH INSTRUCTION
===============================================>>>>>*/

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
int parse_brn(program_t* prog, token_list_t *tlst, instruction_t *inst) {
  // Parse for condition
  char *suffix = remove_first_char(GET_STR(0));
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
  inst->i.brn.padA = 0x1010;
  inst->i.brn.offset = offset;

  return EC_OK;
}

/*=============================================>>>>>
= SPECIAL INSTRUCTIONS
===============================================>>>>>*/


int parse_lsl(program_t* prog, token_list_t *tlst, instruction_t *inst) {
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
int parse_lsl_conversion(program_t *prog, token_list_t *tlst, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  // Create new instruction in memory, to be parsed by sdt
  token_t mod_tkns[] = {
    {T_OPCODE, "mov"},
    GET_TKN(1),
    GET_TKN(2),
    GET_TKN(1),
    GET_TKN(2),
    {T_SHIFT, "lsl"},
    GET_TKN(3)
  };
  token_list_t mod_tlst = {mod_tkns, 7};

  return parse_sdt(prog, &mod_tlst, inst);
}

int parse_halt(program_t *prog, token_list_t *tlst, instruction_t *inst) {
  inst->type = HAL;
  inst->i.hal.pad0 = 0u;
  return EC_OK;
}

/*=============================================>>>>>
= LABEL INSTRUCTIONS
===============================================>>>>>*/


/**
* Update memory according to reference entry
*
* @param label : current string representation of the label in entry
* @param val : current value of entry
* @param obj : A prog_collection_t object which collects, program, label and addr.
*/
void ref_entry(const label_t label, const address_t val, const void *obj){
  prog_collection_t prog_coll = (prog_collection_t) obj;
  if(label == prog_coll.label){
    prog_coll.prog->out[val] = prog_coll.addr;
  }
}


/**
 * Add a symbol to the symbol table in the program and update reference table
 *
 * @param program : pointer to the program information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int add_symbol(program_t *program, label_t label, address_t addr) {
  if (!smap_put(program->sym_m, label, addr)) {
    return 0; // already in symbol map
  }
  prog_collection_t prog_coll = {program, label, addr};
  // check if symbol exists in ref_map and update/remove accordingly
  if (rmap_exists(program->ref_m, label)) {
    rmap_enum(program->ref_m, ref_entry, prog_coll);
  }

  return 1;
}


/**
 * add a symbol to the reference map stored in the program
 *
 * @param program : pointer to the program information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int add_reference(program_t *program, label_t label, address_t addr) {
  // adds reference to ref_map.
  return !rmap_put(program->ref_m, label, addr);
}


bool is_label(token_list_t *tlst) {
  return GET_TYPE(tlst->n - 1) == T_LABEL;
}

void parse_label(program_t *prog, token_list_t *tlst) {
  char *label = GET_STR(0);
  if(smap_exists(prog->sym_m, label)){
    //ERROR!
  }
  smap_put(prog->sym_m, label, prog->mPC);
}

/**
 *  Translates a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form by calling the appropriate
 *  sub-functions
 *
 *  @param tokens: a pointer to the array of tokens
 *  @param inst: a pointer to the instruction to be stored
 */
int parse(program_t *prog, token_list_t *tlst, instruction_t *inst) {
  // If the assembly line is a label
  if (is_label(tlst)) {
    parse_label(prog, tlst);
    return EC_OK;
  }

  // Get the pointer to the first token - this will be the opcode
  char *opcode = GET_STR(0);

  // Parse a branch instruction and its condition
  if (strncmp(opcode, "b", 1) == 0) {
    return parse_brn(prog, tlst, inst);
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = 0xE; //0b1110

  // Calculate function pointer to parse an instruction from the opcode
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      return oplist[i].parse_func(prog, tlst, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return EC_UNSUPPORTED_OP;
}
