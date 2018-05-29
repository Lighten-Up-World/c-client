#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "arm.h"
#include "io.h"
#include "execute.h"
#include "decode.h"
#include "register.h"



int main(int argc, char **argv) {
  assert(argc > 1);

  state_t *state = calloc(1, sizeof(state_t));
  assert(state != NULL);
  state->pipeline.decoded = calloc(1, sizeof(instruction_t));
  assert(state->pipeline.decoded != NULL);
  readFile(argv[1], state->memory, MEM_SIZE);
  DEBUG_PRINT("\n=========\nEmulating: %s\n=========\n", argv[1]);
  //Setup Pipeline
  setPC(state, 0x8);
  *state->pipeline.decoded = decodeWord(getMemWord(state, 0));
  state->pipeline.fetched = getMemWord(state, 0x4);
  DEBUG_PRINT("Initial Pipeline setup:\n\tPC (0x%08x)\n\tFetched (0x%08x)\n",
              getPC(state), state->pipeline.fetched);
  while(state->pipeline.decoded->type != HAL){
    DEBUG_PRINT("\n---------(P %u)---------\n", getPC(state) / 4);
    DEBUG_CMD(printState(state));
    DEBUG_PRINT("Executing %01x Instruction:\n\t", state->pipeline.decoded->type);
    execute(state);
    *state->pipeline.decoded = decodeWord(state->pipeline.fetched);
    state->pipeline.fetched = getMemWord(state, getPC(state));
    incrementPC(state);
  }
  execute(state); // Execute HAL instruction
  free(state->pipeline.decoded);
  free(state);
  return EXIT_SUCCESS;
}
