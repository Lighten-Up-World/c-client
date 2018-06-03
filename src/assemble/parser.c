#include <string.h>
#include <utils/instructions.h>
#include "parser.h"
#include "tokenizer.h"
#include "../utils/instructions.h"
typedef struct {
  char *op;
  opcode_t op_enum;
  int (*parse_func) (token_t*, instruction_t*);
} op_to_parser;

const int NUM_NON_BRANCH_OPS = 16;
const op_to_parser oplist[] = {
    {"add", ADD, parse_dp},
    {"sub", SUB,  parse_dp},
    {"rsb", RSB, parse_dp},
    {"and", AND,  parse_dp},
    {"eor", TEQ,  parse_dp},
    {"orr", ORR,  parse_dp},
    {"mov", MOV,  parse_dp},
    {"tst", TST,  parse_dp},
    {"teq", TEQ,  parse_dp},
    {"cmp", CMP,  parse_dp},

    {"mul", (opcode_t) 0xE, parse_mul},
    {"mla", (opcode_t) 0xE, parse_mul},

    {"ldr", (opcode_t) 0xE, parse_sdt},
    {"str", (opcode_t) 0xE, parse_sdt},

    {"lsl", (opcode_t) 0xE, parse_lsl},
    {"andeq", (opcode_t) 0xE, parse_halt}
};

int consume_token(token_t *arr, token_type_t type);

char *remove_first_char(char *string) {
  return (string + 1);
}

//TODO: rotated values
/**
 * Get the immediate value from operand2
 * @param operand2: a string containing the immediate value
 * @return the operand
 */
operand_t get_op2(char *operand2) {
  operand_t result;

  // Determine the base being (hex or decimal)
  uint8_t base = (uint8_t) ((strncmp("x", operand2, 1) == 0) ? 10 : 16);

  // Strip leading hash from immediate value and convert from string
  uint64_t raw_val = (uint64_t) strtoul(remove_first_char(operand2), NULL, base);

  // Num cannot be represented if it is larger than 32 bits
  uint64_t big_mask = UINT64_MAX - UINT32_MAX;
  if ((raw_val & big_mask) != 0) {
    // Throw error, we can't represent a number this big
    perror("number cannot be represented");
  }

  // If num is bigger than 8 bits we must use a rotate
  uint16_t small_mask = UINT16_MAX - UINT8_MAX;
  if ((raw_val & small_mask) != 0) {
    // Rotate is needed
    result.imm.rotated.value = 0; //TODO
    result.imm.rotated.rotate = 0; //TODO
  } else {
    result.imm.fixed = (uint8_t) raw_val;
  }

  return result;
}

int parse_dp(token_t *tokens, instruction_t *inst) {
  inst->type = DP;

  // Get opcode enum
  char *opcode = tokens[0].str;
  opcode_t op_enum;
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      op_enum = oplist[i].op_enum;
    }
  }

  // Set Rd as second token
  reg_address_t rd = (reg_address_t) atoi(remove_first_char(tokens[1].str));

  // Set whether the CPSR flags should be set, and the location of operand2
  bool S;
  int op2_pos;
  reg_address_t rn;
  if (strcmp("tst", opcode) == 0 || strcmp("teq", opcode) == 0 || strcmp("cmp", opcode) == 0) {
    S = 1;
    op2_pos = 2;
    rn = (reg_address_t) atoi(remove_first_char(tokens[1].str));
  } else {
    if (strcmp("mov", opcode) == 0) {
      op2_pos = 2;
    } else {
      op2_pos = 3;
    }
    S = 0;
  }

  // TODO: add support for shifted registers (optional)
  // Get operand 2
  char *operand2 = tokens[op2_pos].str;
  operand_t op2 = get_op2(operand2);
  bool I = 1; //change to 0 for a shifted register

  inst->i.dp.padding = 0x00;
  inst->i.dp.I = I;
  inst->i.dp.opcode = op_enum;
  inst->i.dp.S = S;
  inst->i.dp.rn = rn;
  inst->i.dp.rd = rd;
  inst->i.dp.operand2 = op2;

  return 0;
}

int parse_mul(token_t *tokens, instruction_t *inst) {
  inst->type = MUL;

  flag_t A;
  flag_t S = false;

  reg_address_t rd = (reg_address_t) atoi(remove_first_char(tokens[1].str));
  reg_address_t rn = (reg_address_t) atoi(remove_first_char(tokens[2].str));
  reg_address_t rs = (reg_address_t) atoi(remove_first_char(tokens[3].str));
  reg_address_t rm = 0;

  if (strcmp(tokens[0].str, "mul") == 0) {
    A = 0;
  } else {
    A = 1;
    rm = (reg_address_t) atoi(remove_first_char(tokens[0].str));
  }

  inst->i.mul.pad0 = (byte_t) 0x0;
  inst->i.mul.A = A;
  inst->i.mul.S = S;
  inst->i.mul.rd = rd;
  inst->i.mul.rn = rn;
  inst->i.mul.rs = rs;
  inst->i.mul.pad9 =(byte_t) 0x1001;
  inst->i.mul.rm = rm;

  return 0;
}

//TODO
int parse_sdt(token_t *tokens, instruction_t *inst) {
  return 0;
}
/*
int parse_sdt(token_t *tokens, instruction_t *inst) {
  inst->type = SDT;

  inst->i.sdt.pad0 = 0x01;
  inst->i.sdt.I = I;
  inst->i.sdt.P = P;
  inst->i.sdt.U = U;
  inst->i.sdt.pad0 = 0x0;
  inst->i.sdt.L = L;
  inst->i.sdt.rn = rn;
  inst->i.sdt.rd = rd;
  inst->i.sdt.offset = offset;

  return 0;
}*/

//TODO
int parse_brn(token_t *tokens, instruction_t *inst) {
  return 0;
}
/*
int parse_brn(token_t *tokens, instruction_t *inst) {
  inst->type = BRN;

  // encode condition
  switch(remove_first_char(*tokens[0].str)) {
    case "eq":
    case "ne":
    case "ge":
    case "lt":
    case "gt":
    case "le":
    case "al":
      break;
    default:
      //handle error here
      break;
  }

  // encode offset

  inst->i.brn.padA = (byte_t) 0x1010;
  inst->i.brn.offset = offset;

  return 0;
}*/

int parse_lsl(token_t *tokens, instruction_t *inst) {
  //Treat lsl Rn, <expr> as mov Rn, Rn, lsl <expr>
  reg_address_t r = (reg_address_t) atoi(remove_first_char(tokens[1].str));

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

// Only use this if we support mov with a shifted register in DP - TODO
int parse_lsl_conversion(token_t *tokens, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  // Create new instruction in memory, to be parsed by sdt
  token_t *mod_tokens = malloc(4 * sizeof(token_t));
  mod_tokens[0].type = T_OPCODE;
  mod_tokens[0].str = "mov";
  //etc...

  parse_sdt(mod_tokens, inst);

  // Free up the memory used for the mnodified tokens
  free(mod_tokens);

  return -1;
}

int parse_halt(token_t *tokens, instruction_t *inst) {
  inst->type = HAL;
  inst->i.hal.pad0 = 0u;
  return 0;
}

bool is_label(token_t *tokens) { return false; }
void parse_label() {}

/**
 *  Translates a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form by calling the appropriate
 *  sub-functions
 *
 *  @param tokens: a pointer to the array of tokens
 *  @param inst: a pointer to the instruction to be stored
 */
int parse(token_t *tokens, instruction_t *inst) {
  // If the assembly line is a label
  if (is_label(tokens)) {
    parse_label();
    return 0;
  }

  // Get the pointer to the first token - this will be the opcode
  char *opcode = tokens[0].str;

  // Parse a branch instruction and its condition
  if (strncmp(opcode, "b", 1) == 0) {
    parse_brn(tokens, inst);
    return 0;
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = 0xE; //0b1110

  // Calculate function pointer to parse an instruction from the opcode
  // TODO: find out how to use the map and convert this
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, opcode) == 0) {
      return oplist[i].parse_func(tokens, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return 1;
}
