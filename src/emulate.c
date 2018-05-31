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
  getMemWord(state, 0, &state->pipeline.fetched);
  *state->pipeline.decoded = decodeWord(state->pipeline.fetched);
  getMemWord(state, 0x4, &state->pipeline.fetched);

  DEBUG_PRINT("Initial Pipeline setup:\n\tPC (0x%08x)\n\tFetched (0x%08x)\n",
              getPC(state), state->pipeline.fetched);

  while (state->pipeline.decoded->type != HAL) {
    DEBUG_PRINT("\n---------(PC=%04x PC@%04x)---------\n", getPC(state), getPC(state) - 0x8);
    DEBUG_CMD(printState(state));
    DEBUG_PRINT("Executing: %01x Instruction:\n\t", state->pipeline.decoded->type);

    if (!execute(state)) {
      *state->pipeline.decoded = decodeWord(state->pipeline.fetched);
      DEBUG_PRINT("\nDecoded: %08x\n", state->pipeline.fetched);
      getMemWord(state, getPC(state), &state->pipeline.fetched);
      DEBUG_PRINT("\nFetching@%04x: %08x\n", getPC(state), state->pipeline.fetched);
    }
    incrementPC(state);
    DEBUG_PRINT("\n---------(PC=%04x)---------\n\n", getPC(state));
  }

  // Execute HAL instruction
  execute(state);
  free(state->pipeline.decoded);
  free(state);
  return EXIT_SUCCESS;
}
