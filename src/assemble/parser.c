/*
 * Contains function parse which, given the tokens for a program line, generates
 * an instruction_t struct (defined in instructions.h), making it simpler
 * to encode.
 */

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

// TODO: remove magic nums

const suffix_to_num brn_suffixes[NUM_BRN_SUFFIXES] = {
    {"eq", 0x0},
    {"ne", 0x1},
    {"ge", 0xA},
    {"lt", 0xB},
    {"gt", 0xC},
    {"le", 0xD},
    {"al", 0xE},
    {"",   0xE}
};

const suffix_to_num shifts[NUM_SHIFT_SUFFIXES] = {
    {"asr", ASR},
    {"lsl", LSL},
    {"lsr", LSR},
    {"ror", ROR}
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


//// UTILITY FUNCTIONS ////

bool is_label(list_t *tklst) {
  return token_list_get_type(tklst, tklst->len - 1) == T_LABEL;
}

char *remove_first_char(char *string) {
  return (string + 1);
}

reg_address_t parse_register(list_t *tklst, reg_address_t reg) {
  return atoi(remove_first_char(token_list_get_str(tklst, reg)));
}

bool compare_op(char *str, char *opcode) {
  return strcmp(str, opcode) == 0;
}

int str_to_enum(char *opcode) {
  opcode_t op_enum;
  int isSet = 0;
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (compare_op(opcode, oplist[i].op)) {
      op_enum = oplist[i].op_enum;
      isSet = 1;
    }
  }
  if (!isSet) return -1;
  return op_enum;
}

word_t parse_expression(list_t *tklst, int idx) {
  char *str_expr = remove_first_char(token_list_get_str(tklst, idx));
  return (word_t) strtol(str_expr, NULL, 0);
}

// Create word-reference pair data structure
wordref_t *wordref_new(word_t word, address_t ref) {
  wordref_t *self = malloc(sizeof(wordref_t));
  if (self == NULL) {
    return NULL;
  }
  self->word = word;
  self->ref = ref;
  return self;
}

//// OPERAND/OFFSET INSTRUCTIONS ////

/**
 * Make the rotation of a 24 bit value
 *
 * @param: value to rotate
 * @return: immediate rotated value to be stored in an instruction
 */
int make_rotation(op_rotate_immediate_t *op_rot_imm, word_t value) {
  byte_t rot = 0;

  while (get_bits(value, 31, 8) != 0 && rot < MAX_ROT_VAL) {
    value = rotate_left(value, 2);
    rot += 1;
  }

  if (rot == MAX_ROT_VAL) {
    perror("Cannot convert value");
    return EC_INVALID_PARAM;
  }

  op_rot_imm->value = (byte_t) value;
  op_rot_imm->rotate = rot;
  return EC_OK;
}

/**
 * TODO: comment this function (DAN)
 *
 * Case 1: <Operand2> := <register>
 * Case 2a: <Operand2> := <register>, <shiftname> <register>
 * Case 2b: <Operand2> := <register>, <shiftname> <#expression>
 * Case 3: <Operand2> := <register>, Rn
 */
int parse_shifted_reg(list_t *tklst, operand_t *op, int start) {
     int operand_size = tklst->len - start;

  // Case 1: <Operand2> := <register>
  if(operand_size == 1){
         op->reg.rm = parse_register(tklst, start);
    op->reg.type = LSL;
    op->reg.shiftBy = 0;
    op->reg.shift.constant.integer = 0;
    return EC_OK;
  }
  // Case 3: <Operand2> := <register>, Rn
  if(operand_size == 2){
      op->reg.rm = parse_register(tklst, start);
      op->reg.type = LSL;
             op->reg.shift.shiftreg.rs = parse_register(tklst, start + 1);
      op->reg.shift.shiftreg.zeroPad = 0;
      op->reg.shiftBy = 0;
      return EC_OK;
  }
  //Case 2: <register>, <shiftname> <register>
  //      | <register>, <shiftname> <#expression>
  if(operand_size >= 4){
         char *shiftname = token_list_get_str(tklst, start+2);
    for (int i = 0; i < NUM_SHIFT_SUFFIXES; i++) {
      if (strcmp(shifts[i].suffix, shiftname) == 0) {
        op->reg.type = shifts[i].num;
      }
    }
    op->reg.rm = parse_register(tklst, start);

    // Case 2a: <Operand2> := <register>, <shiftname> <register>
    if(token_list_get_type(tklst, start + 3) == T_REGISTER){
             op->reg.shiftBy = 1;
      op->reg.shift.shiftreg.rs = parse_register(tklst, start + 3);
      op->reg.shift.shiftreg.zeroPad = 0;
      return EC_OK;
    }

    // Case 2b: <Operand2> := <register>, <shiftname> <#expression>
    if(token_list_get_type(tklst, start + 3) == T_HASH_EXPR){
             op->reg.shiftBy = 0;
      op->reg.shift.constant.integer
        = parse_expression(tklst, start + 3);
        return EC_OK;
    }
  }

     return EC_UNSUPPORTED_OP;
}

/**
 * TODO: comment this function (DAN)
 */
int parse_operand(list_t *tklst, instruction_t *instr, int start) {
  operand_t *op = &instr->i.dp.operand2;

  // Case 1: <Operand2> := #expression,
  if (instr->i.dp.I) {
    char *operand2 = token_list_get_str(tklst, start);
    // Strip leading hash sign
    char *strVal = remove_first_char(operand2);

    // Determine the base (hex or decimal) and convert to int
    uint8_t
        base = (uint8_t)((strncmp("0x", strVal, 2) == 0) ? HEX_BASE : DEC_BASE);

    uint64_t raw_val = (uint64_t) strtoul(strVal, NULL, base);
    // Num cannot be represented if it is larger than 32 bits
    uint64_t big_mask = UINT64_MAX - UINT32_MAX;
    if ((raw_val & big_mask) != 0) {
      perror("number cannot be represented");
      exit(EXIT_FAILURE);
    }
    // If num is bigger than 8 bits we must use a rotate
    uint32_t small_mask = UINT32_MAX - UINT8_MAX;
    if ((raw_val & small_mask) != 0) {
      return make_rotation(&op->imm.rotated, (uint32_t) raw_val);
    } else {
      op->imm.rotated.value = (uint8_t) raw_val;
      op->imm.rotated.rotate = 0;
      return EC_OK;
    }
  }
    // Case 2: <Operand2> := Rm{,<shift>}
  else {
    return parse_shifted_reg(tklst, op, start);
  }
}

/**
 * TODO: comment this function (DAN)
 *
 * Case 1: <=expression>          {4}
 * Case 2: [Rn]                   {6}
 * Case 3: [Rn,<#expression>]     {8}
 * Case 4: [Rn],<#expression>     {8}
 * Case 5: [Rn, {+/-}Rm{,<shift>}]
 * Case 6: [Rn],{+/-}Rm{,<shift>
 */
int parse_offset(assemble_state_t *prog, instruction_t *instr, int start) {

  int value = 0;
  // Case 1: =expr -> Re-execute with mov or ldr
  if (prog->tklst->len == NUM_TOKS_EQ_EXPR) {
    value = parse_expression(prog->tklst, 3);

    if (value <= MAX_HEX) {
      char *immVal = token_list_get_str(prog->tklst, 3);
      immVal[0] = '#';
      list_t *mod_tklst = token_list_new();
      token_list_add_pair(mod_tklst, T_OPCODE, "mov");
      token_list_add(mod_tklst, token_list_get(prog->tklst, 1));
      token_list_add(mod_tklst, token_list_get(prog->tklst, 2));
      token_list_add_pair(mod_tklst, T_HASH_EXPR, immVal);

      token_list_delete(prog->tklst);
      prog->tklst = mod_tklst;

      return parse_dp(prog, instr);
    } else {
      wordref_t *addon = wordref_new(value, prog->mPC);
      if (addon == NULL) {
        return EC_NULL_POINTER;
      }
      list_add(prog->additional_words, addon);

      list_t *mod_tklst = token_list_new();
      token_list_add_pair(mod_tklst, T_OPCODE, "ldr");
      token_list_add(mod_tklst, token_list_get(prog->tklst, 1));
      token_list_add(mod_tklst, token_list_get(prog->tklst, 2));
      token_list_add_pair(mod_tklst, T_L_BRACKET, "[");
      token_list_add_pair(mod_tklst, T_REGISTER, "r15");
      token_list_add_pair(mod_tklst, T_COMMA, ",");
      token_list_add_pair(mod_tklst, T_HASH_EXPR, "#0xFFF");
      token_list_add_pair(mod_tklst, T_R_BRACKET, "]");

      token_list_delete(prog->tklst);
      prog->tklst = mod_tklst;

      return parse_sdt(prog, instr);
    }
  }
  // Case 2: [Rn]
  if (prog->tklst->len == NUM_TOKS_PRE_IND_ADDR) {
    instr->i.sdt.I = 0;
    instr->i.sdt.P = 1;
    instr->i.sdt.U = 1;
    instr->i.sdt.rn = parse_register(prog->tklst, 4);
    instr->i.sdt.offset.imm.fixed = 0;
    return EC_OK;
  }

  // Case 3/4: [Rn{]}, #expression {]}
  if (prog->tklst->len == NUM_TOKS_HASH_EXPR) {
    instr->i.sdt.rn = parse_register(prog->tklst, 4);
    instr->i.sdt.U = 1;
    instr->i.sdt.I = 0;

    // Case 3: [Rn, #expression]
    if (token_list_get_type(prog->tklst, 6) == T_HASH_EXPR) {
      instr->i.sdt.P = 1;
      word_t result = parse_expression(prog->tklst, 6);
      if (is_negative(result)) {
        result = negate(result);
        instr->i.sdt.U = 0;
      }
      instr->i.sdt.offset.imm.fixed = result;
      return EC_OK;
    }


    if (token_list_get_type(prog->tklst, 5) == T_R_BRACKET) {
      instr->i.sdt.P = 0;
      // Case 4: [Rn],<#expression>
      if (token_list_get_type(prog->tklst, 7) == T_HASH_EXPR) {
        instr->i.sdt.offset.imm.fixed = parse_expression(prog->tklst, 7);
        return EC_OK;
      }
    }
  }
  // Case 5: [Rn, {+/-}Rm{,<shift>}]
  if (token_list_get_type(prog->tklst, 5) == T_COMMA) {
    instr->i.sdt.rn = parse_register(prog->tklst, 4);
    instr->i.sdt.P = 1;
    instr->i.sdt.I = 1;
    int shifted_reg_start = 6;
    if (token_list_get_type(prog->tklst, 6) == T_PLUS
        || token_list_get_type(prog->tklst, 6) == T_MINUS) {
      shifted_reg_start++;
      if (token_list_get_type(prog->tklst, 6) == T_MINUS) {
        instr->i.sdt.U = 0;
      }
    }
    int ec =
        parse_shifted_reg(prog->tklst, &instr->i.sdt.offset, shifted_reg_start);

    return ec;
  }
  // Case 6: [Rn],{+/-}Rm{,<shift>}
  if (token_list_get_type(prog->tklst, 5) == T_R_BRACKET) {
    instr->i.sdt.rn = parse_register(prog->tklst, 4);
    instr->i.sdt.P = 0;
    instr->i.sdt.I = 1;
    return parse_shifted_reg(prog->tklst, &instr->i.sdt.offset, 7);
  }

  return EC_UNSUPPORTED_OP;
}

/**
 * Parse a DP instruction
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_dp(assemble_state_t *prog, instruction_t *instr) {

  // Get opcode enum
  char *opcode = token_list_get_str(prog->tklst, 0);
  opcode_t op_enum = (opcode_t) str_to_enum(opcode);

  // Set whether the CPSR flags should be set
  bool S = compare_op(opcode, "tst") || compare_op(opcode, "teq")
      || compare_op(opcode, "cmp");

  // Set position of rn and position of operand2
  int rn_pos = S || compare_op(opcode, "mov") ? 1 : 3;

  // Set all instruction fields
  instr->type = DP;
  instr->cond = AL_COND_CODE; //0b1110
  instr->i.dp.padding = 0x00;
  instr->i.dp.I = token_list_get_type(prog->tklst, rn_pos + 2) == T_HASH_EXPR;
  instr->i.dp.opcode = op_enum;
  instr->i.dp.S = S;
  instr->i.dp.rn =
      compare_op(opcode, "mov") ? 0 : parse_register(prog->tklst, rn_pos);
  instr->i.dp.rd = S ? 0 : parse_register(prog->tklst, RD_POS);

  return parse_operand(prog->tklst, instr, rn_pos + 2);
}

/**
 * Parse a MUL instruction
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_mul(assemble_state_t *prog, instruction_t *inst) {
  flag_t A = (flag_t) strcmp(token_list_get_str(prog->tklst, 0), "mul");

  reg_address_t rd = parse_register(prog->tklst, RD_POS);
  reg_address_t rm = parse_register(prog->tklst, RM_POS);
  reg_address_t rs = parse_register(prog->tklst, RS_POS);
  reg_address_t rn = A ? parse_register(prog->tklst, RN_POS) : 0;

  inst->type = MUL;
  inst->cond = AL_COND_CODE;
  inst->i.mul.pad0 = 0x0;
  inst->i.mul.A = A;
  inst->i.mul.S = 0x0;
  inst->i.mul.rd = rd;
  inst->i.mul.rn = rn;
  inst->i.mul.rs = rs;
  inst->i.mul.pad9 = HEX_NINE;
  inst->i.mul.rm = rm;

  return EC_OK;
}

/**
 * Parse an SDT instruction
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_sdt(assemble_state_t *prog, instruction_t *instr) {
  char *opcode = token_list_get_str(prog->tklst, 0);
  flag_t L = compare_op(opcode, "ldr");
  if (!L && !compare_op(opcode, "str")) {
    perror("Opcode not recognised\n");
  }
  instr->type = SDT;
  instr->cond = AL_COND_CODE; //0b1110
  instr->i.sdt.L = L;
  instr->i.sdt.pad1 = 0x1;
  instr->i.sdt.pad0 = 0x0;
  instr->i.sdt.rd = parse_register(prog->tklst, RD_POS);

  return parse_offset(prog, instr, 3);
}

/**
 * Calculate the offset of an address from the current program_state Counter,
 * to be stored in a BRN instruction
 *
 * @param address: address to calculate the offset of
 * @param PC: address of the branch instruction being executed
 * @return: offset to be stored in instruction - a shifted 24 bit value
 */
word_t calculate_offset(int32_t address, word_t PC) {
  int32_t offset = address - PC - 8;

  if (offset > 0x3FFFFFF || offset < -0x3FFFFFF) {
    perror("Offset was too large to store");
    exit(EC_INVALID_PARAM);
  }

  // Shift right by 2 then store (in 24 bits)
  shift_result_t shifted_offset = a_shift_right_c(offset, 2);
  if (shifted_offset.carry == true) {
    perror("carry occurred when trying to store offset");
    exit(EC_INVALID_PARAM);
  }

  return shifted_offset.value;
}

/**
 * Parse a BRN instruction
 *
 * Syntax is: b<code> <expression>
 * <code>       - eq|ne|ge|lt|gt|al|
 * <expression> - Label name whose address is retreived
 *
 * The <expression> is the target address, which may be a label.
 * The assembler should compute the offset between the current address and
 * the label, taking into account the off-by-8 bytes effect that will occur
 * due to the ARM pipeline. This signed offset should be 26 bits in length,
 * before being shifted right two bits and having the lower 24 bits stored
 * in the Offset field.
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_brn(assemble_state_t *prog, instruction_t *inst) {
  // Parse for condition
  char *suffix = remove_first_char(token_list_get_str(prog->tklst, 0));
  for (int i = 0; i < NUM_BRN_SUFFIXES; i++) {
    if (strcmp(brn_suffixes[i].suffix, suffix) == 0) {
      inst->cond = brn_suffixes[i].num;
    }
  }

  word_t offset;
  if (token_list_get_type(prog->tklst, 1) == T_STR) {
    // Check if label is already in map, if so get address
    char *label = token_list_get_str(prog->tklst, 1);
    if (smap_exists(prog->smap, label)) {
      address_t addr = 0;
      smap_get_address(prog->smap, label, &addr);
      offset = calculate_offset(addr, prog->mPC);
    } else {
      rmap_put(prog->rmap, label, prog->mPC);
      offset = 0xFFFFFF; // dummy value
    }
  } else {
    offset =
        calculate_offset(atoi(token_list_get_str(prog->tklst, 0)), prog->mPC);
  }

  inst->type = BRN;
  inst->i.brn.padA = 0xA;
  inst->i.brn.offset = offset;

  return EC_OK;
}

//// SPECIAL INSTRUCTIONS ////

/**
 * Parse a logical shift left instruction
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_lsl(assemble_state_t *prog, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  // Create new instruction in memory, to be parsed by sdt
  list_t *mod_tklst = token_list_new();
  token_list_add_pair(mod_tklst, T_OPCODE, "mov");
  token_list_add(mod_tklst, token_list_get(prog->tklst, 1));
  token_list_add(mod_tklst, token_list_get(prog->tklst, 2));
  token_list_add(mod_tklst, token_list_get(prog->tklst, 1));
  token_list_add(mod_tklst, token_list_get(prog->tklst, 2));
  token_list_add_pair(mod_tklst, T_SHIFT, "lsl");
  token_list_add(mod_tklst, token_list_get(prog->tklst, 3));

  token_list_delete(prog->tklst);
  prog->tklst = mod_tklst;


  return parse_dp(prog, inst);
}

/**
 * Parse the HAL instruction
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse_halt(assemble_state_t *prog, instruction_t *inst) {
  inst->type = HAL;
  inst->cond = 0;
  inst->i.hal.pad0 = 0;
  return EC_OK;
}

//// LABEL INSTRUCTIONS ////

/**
 * Parse a line containing a branch with a label
 *
 * @param prog: pointer to the program state
 * @return: integer error code based on success of function
 */
int parse_label(assemble_state_t *prog) {
  int _status = EC_OK;
  char *label = token_list_get_str(prog->tklst, 0);
  if (smap_exists(prog->smap, label)) {
    return EC_IS_LABEL;
  }
  smap_put(prog->smap, label, prog->mPC);
  if (rmap_exists(prog->rmap, label)) {
    int num_references = rmap_get_references(prog->rmap, label, NULL, 0);
    size_t size_ref = num_references * sizeof(address_t);
    address_t *addrs = malloc(size_ref);
    if (addrs == NULL) {
      perror("parse_label(): malloc failed");
      return EC_NULL_POINTER;
    }
    if ((_status = rmap_get_references(prog->rmap, label, addrs, size_ref))) {
      return _status;
    }
    for (int i = 0; i < num_references; i++) {
      word_t offset = calculate_offset(prog->mPC, addrs[i]) | 0xFF000000;
      word_t curr;
      get_word(prog->out, addrs[i], &curr);
      curr &= offset;
      set_word(prog->out, addrs[i], curr);
    }
  }
  return _status;
}

/**
 *  Translate a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form by calling the appropriate
 *  sub-functions
 *
 * @param prog: pointer to the program state
 * @param inst: pointer to the Instruction to store information in
 * @return: integer error code based on success of function
 */
int parse(assemble_state_t *prog, instruction_t *inst) {
  // If the assembly line is a label
  if (is_label(prog->tklst)) {
    parse_label(prog);
    return EC_SKIP;
  }

  // Get the pointer to the first token - this will be the opcode
  char *opcode = token_list_get_str(prog->tklst, 0);

  // Parse a branch instruction and its condition
  if (!strncmp(opcode, "b", 1)) {
    return parse_brn(prog, inst);
  }

  if (!strcmp("lsl", opcode)) {
    return parse_lsl(prog, inst);
  }

  // Calculate function pointer to parse an instruction from the opcode
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      return oplist[i].parse_func(prog, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return EC_UNSUPPORTED_OP;
}
