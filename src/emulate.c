/*
 * Initialises the memory and registers and loops through the instructions,
 * executing them until the halt instruction or end of memory is reached.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/instructions.h"
#include "emulate.h"

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

  //Setup Pipeline
  set_pc(state, 0x8);
  get_mem_word(state, 0, &state->pipeline.fetched);
  *state->pipeline.decoded = decode_word(state->pipeline.fetched);
  get_mem_word(state, 0x4, &state->pipeline.fetched);

  while (state->pipeline.decoded->type != HAL) {

    if (!execute(state)) {
      *state->pipeline.decoded = decode_word(state->pipeline.fetched);
             get_mem_word(state, get_pc(state), &state->pipeline.fetched);
           }
    increment_pc(state);
       }

  // Execute HAL instruction
  execute(state);
  free(state->pipeline.decoded);
  free(state);
  return EXIT_SUCCESS;
}
