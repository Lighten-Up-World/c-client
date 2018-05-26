#include "arm.h"
#include "bitops.h"
/**
* Checks if the condition on the decoded instruction is met using the current
* state of the flags register (CPSR).
*
* @param state The current VM state
* @returns 1 when condition is met, 0 if not.
*/
int condition(state_t *state, condition_t cond){
  byte_t flags = getNibble(state->registers.cpsr, sizeof(word_t));

  switch(cond){
    case EQ:
      return ;
    case NE:
      return ;
    case GE:
      return ;
    case LT:
      return ;
    case GT:
      return ;
    case LE:
      return ;
    case AL:
      return ;
    default:
      fprintf(stderr, "Incorrect cond flag %x\n", cond);
      //free_state();
      return 0;
  }
}

void execute(state_t *state);
void executeDPI(state_t *state);
void executeMUL(state_t *state);
void executeBRN(state_t *state);
void executeSDT(state_t *state);
void executeHAL(state_t *state);


//// DP ////


//// MUL ////


//// SDT ////


//// BRN ////


//// HAL ////


//// EXECUTE ENTRY FUNCTION ////

// switch (opcode) {
//   case AND:
//   case TST:
//     res = op1 & op2
//   break;
//   case EOR
// }
//
//
