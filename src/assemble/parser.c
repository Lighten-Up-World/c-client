#include <utils/instructions.h>
#include "tokenizer.h"
#include "../utils/instructions.h"

int consume_token(token_t *arr, token_type_t type);

/*typedef struct {
  instruction_type_t type;
  byte_t cond : 4;
  instructions_t i;
} instruction_t;*/

int parse_dp(token_t *tkns, instruction_t *inst) {
  inst->type = DP;
  inst->i = {
      padding,
      I,
      opcode,
      S,
      rn,
      rd,
      operand2
  };
}

int parse_mul(token_t *tkns, instruction_t *inst) {
  inst->type = MUL;
  inst->i = {
      0x0,
      A,
      S,
      rd,
      rn,
      rs,
      0x1001,
      rm
  };
}

int parse_sdt(token_t *tkns, instruction_t *inst) {
  inst->type = SDT;
  inst->i = {
      0x01,
      I,
      P,
      U,
      0x0,
      L,
      rn,rd,
      offset
  };
}

int parse_brn(token_t *tkns, instruction_t *inst) {
  inst->type = BRN;
  inst->i = {
      0x1010,
      offset
  };
}

int parse_lsl(token_t *tkns, instruction_t *inst) {
  // convert this then call...
}


int parse_halt(token_t *tkns, instruction_t *inst) {
  inst->type = HAL;
  inst->i = {0u};
  return 0;
}

bool is_label(token_t *tkns) { return true; }
void parse_label();

// TODO: find out how to use the map and convert this
typedef struct {
  char op[];
  int (*parse_func) (token_t*, instruction_t*);
} op_to_parser;

const NUM_NON_BRANCH_OPS = 16;
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
  if (*opcode[0] == 'b') {
    parse_brn(tokens, inst);
    return 0;
  }

  // Not a branch instruction so set condition to always execute
  inst->cond = 0xE; //0b1110

  // Calculate function pointer to parse an instruction from the opcode
  // TODO: find out how to use the map and convert this
  for (int i = 0; i < NUM_NON_BRANCH_OPS; i++) {
    if (oplist[i].op == *opcode) {
      return oplist[i].parse_func(tokens, inst);
    }
  }

  // Throw an error here, unsupported opcode
  return 1;
}
