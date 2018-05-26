#include <stdio.h>
#include "execute.h"
#include "bitops.h"

/**
* Checks if the condition on the decoded instruction is met using the current
* state of the flags register (CPSR).
*
* @param state: The current VM state
* @param cond: The condition extracted from the instructionn
* @returns 1 when condition is met, 0 if not.
*/
int condition(state_t *state, byte_t cond){
  byte_t flags = getNibble(state->registers.cpsr, sizeof(word_t));

  switch(cond){
    case EQ:
      return flags & Z;
    case NE:
      return !(flags & Z);
    case GE:
      return (flags & V) == ((flags & N) >> 3);
    case LT:
      return (flags & V) != ((flags & N) >> 3);
    case GT:
      return !(flags & Z) && ((flags & V) == ((flags & N) >> 3));
    case LE:
      return (flags & Z) || ((flags & V) != ((flags & N) >> 3));
    case AL:
      return 1;
    default:
      fprintf(stderr, "Invalid cond flag %x\n", cond);
      //free_state();
      return 0;
  }
}

/**
*
*/
void execute(state_t *state){
  instruction_t *decoded = state->pipeline.decoded_instruction;
  if(decoded->type == HAL){
    executeHAL(state);
    return;
  }
  if(condition(state, decoded->cond)){
    switch(decoded->type){
      case DP:
        executeDP(state, decoded->i.dp);
        break;
      case MUL:
        executeMUL(state, decoded->i.mul);
        break;
      case SDT:
        executeSDT(state, decoded->i.sdt);
        break;
      case BRN:
        executeBRN(state, decoded->i.brn);
        break;
      default:
        fprintf(stderr, "Invalid cond flag %x\n", cond);
        //free_state();
    }
  }
}
void executeDP(state_t *state, dp_instruction_t   instr);
void executeMUL(state_t *state, mul_instruction_t instr);
void executeBRN(state_t *state, brn_instruction_t instr);
void executeSDT(state_t *state, sdt_instruction_t instr);
void executeHAL(state_t *state);
