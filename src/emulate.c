/*
 * Initialises the memory and registers and loops through the instructions,
 * executing them until the halt instruction or end of memory is reached.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/io.h"
#include "emulate/execute.h"
#include "emulate/decode.h"
#include "utils/register.h"

int main(int argc, char **argv) {
  assert(argc > 1);

  state_t *state = calloc(1, sizeof(state_t));
  assert(state != NULL);
  state->pipeline.decoded = calloc(1, sizeof(instruction_t));
  assert(state->pipeline.decoded != NULL);
  read_file(argv[1], state->memory, MEM_SIZE);
  DEBUG_PRINT("\n=========\nEmulating: %s\n=========\n", argv[1]);

  //Setup Pipeline
  set_pc(state, 0x8);
  get_mem_word(state, 0, &state->pipeline.fetched);
  *state->pipeline.decoded = decode_word(state->pipeline.fetched);
  get_mem_word(state, 0x4, &state->pipeline.fetched);

  DEBUG_PRINT("Initial Pipeline setup:\n\tPC (0x%08x)\n\tFetched (0x%08x)\n",
              get_pc(state), state->pipeline.fetched);

  while (state->pipeline.decoded->type != HAL) {
    DEBUG_PRINT("\n---------(PC=%04x PC@%04x)---------\n", get_pc(state), get_pc(state) - 0x8);
    DEBUG_CMD(printState(state));
    DEBUG_PRINT("Executing: %01x Instruction:\n\t", state->pipeline.decoded->type);

    if (!execute(state)) {
      *state->pipeline.decoded = decode_word(state->pipeline.fetched);
      DEBUG_PRINT("\nDecoded: %08x\n", state->pipeline.fetched);
      get_mem_word(state, get_pc(state), &state->pipeline.fetched);
      DEBUG_PRINT("\nFetching@%04x: %08x\n", get_pc(state), state->pipeline.fetched);
    }
    increment_pc(state);
    DEBUG_PRINT("\n---------(PC=%04x)---------\n\n", get_pc(state));
  }

  // Execute HAL instruction
  execute(state);
  free(state->pipeline.decoded);
  free(state);
  return EXIT_SUCCESS;
}
