#include <memory.h>
#include <complex.h>
#include "tokenizer.h"
#include "../utils/instructions.h"

int consume_token(token_t *arr, token_type_t type);

char remove_first_char(char *string) {
  return *(string + 1);
}

int parse_dp(token_t *tokens, instruction_t *inst) {
  inst->type = DP;

  const char *opcode[] = tokens[0].str;

  reg_address_t rn;
  reg_address_t rd;

  // Set Rd as second token
  rd = (reg_address_t) remove_first_char(*tokens[1].str); //cast to integer for reg-address

  // Set whether the CPSR flags should be set
  bool S;
  if (strcmp("tst", opcode) == 0 || strcmp("teq", opcode) == 0 || strcmp("cmp", opcode) == 0) {
    S = 1;
    rn = (reg_address_t) remove_first_char(*tokens[1].str);
  } else {
    S = 0;
  }

  inst->i.dp.padding = 0x00;
  inst->i.dp.I = I;
  inst->i.dp.opcode = opcode; //do we have to use the enum for this
  inst->i.dp.S = S;
  inst->i.dp.rn = rn;
  inst->i.dp.rd = rd;
  inst->i.dp.operand2 = operand2;

  return 0;
}



int parse_mul(token_t *tokens, instruction_t *inst) {
  inst->type = MUL;

  flag_t A;
  flag_t S = false;

  //check these work - need to cast to integer from string first
  reg_address_t rd = (reg_address_t) remove_first_char(*tokens[1].str);
  reg_address_t rn = (reg_address_t) remove_first_char(*tokens[2].str);
  reg_address_t rs = (reg_address_t) remove_first_char(*tokens[3].str);
  reg_address_t rm = 0;

  if (strcmp(*tokens[0].str, "mul") == 0) {
    A = 0;
  } else {
    A = 1;
    rm = (reg_address_t) remove_first_char(*tokens[0].str);
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
}

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
}

int parse_lsl(token_t *tokens, instruction_t *inst) {
  //lsl Rn, <expr> === mov Rn, Rn, lsl <expr>

  //might need to malloc here to avoid changing ROM
  //where do we free?

  token_t *mod_tokens;
  parse_sdt(mod_tokens, inst);

  return -1;
}

int parse_halt(token_t *tokens, instruction_t *inst) {
  inst->type = HAL;
  inst->i.hal.pad0 = 0u;
  return 0;
}

bool is_label(token_t *tokens) { return true; }
void parse_label();

// TODO: find out how to use the map and convert this
typedef struct {
  char *op[];
  int (*parse_func) (token_t*, instruction_t*);
} op_to_parser;

const int NUM_NON_BRANCH_OPS = 16;
const op_to_parser oplist[] = {
    {"add", parse_dp},
    {"sub", parse_dp},
    {"rsb", parse_dp},
    {"and", parse_dp},
    {"eor", parse_dp},
    {"orr", parse_dp},
    {"mov", parse_dp},
    {"tst", parse_dp},
    {"teq", parse_dp},
    {"cmp", parse_dp},

    {"mul", parse_mul},
    {"mla", parse_mul},

    {"ldr", parse_sdt},
    {"str", parse_sdt},

    {"lsl", parse_lsl},
    {"andeq", parse_halt}
};

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
  char *opcode[] = tokens[0].str;

  // Parse a branch instruction and its condition
  if (strcmp(*opcode[0], 'b') == 0) {
    parse_brn(tokens, inst);
    return 0;
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = 0xE; //0b1110

  // Calculate function pointer to parse an instruction from the opcode
  // TODO: find out how to use the map and convert this
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (strcmp(oplist[i].op, *opcode) == 0) {
      return oplist[i].parse_func(tokens, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return 1;
}
