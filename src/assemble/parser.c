#include <utils/instructions.h>
#include "tokenizer.h"
#include "../utils/instructions.h"

int consume_token(token_t *arr, token_type_t type);

int parse_dp(token_t *tkns, instruction_t *inst);
int parse_mul(token_t *tkns, instruction_t *inst);
int parse_sdt(token_t *tkns, instruction_t *inst);
int parse_brn(token_t *tkns, instruction_t *inst);

void parse_label();

/**
 *  Translates a list of tokens comprising a line of assembly
 *  into its corresponding instruction_t form.
 *
 *  @param tokens: a pointer to the array of tokens
 *  @param inst: a pointer to the instruction to be stored
 */
int parse(token_t *tokens, instruction_t *inst) {
  // If the line is a label:
  if (is_label) {
    parse_label();
    return 0;
  }

  // Set the cond (common to all instructions)
  inst->cond = tokens.cond;

  // Parse a particular instruction by its opcode
  switch(inst->opcode) {
    case ADD:
    case MUL:
    case SUB:
      parse_dp(tokens, inst);

    etc...
  } // look for a nicer way to do this with function pointers

}