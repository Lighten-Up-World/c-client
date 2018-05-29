#include <stdlib.h>
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

  //Setup Pipeline
  setPC(state, 0x8);
  word_t toDecode;
  getMemWord(state, 0, &toDecode);
  *state->pipeline.decoded = decodeWord(toDecode);
  word_t fetched;
  getMemWord(state, 0x4, &fetched);
  state->pipeline.fetched = fetched;

  while(state->pipeline.decoded->type != HAL){
    execute(state);
    *state->pipeline.decoded = decodeWord(state->pipeline.fetched);
    getMemWord(state, getPC(state), &fetched);
    state->pipeline.fetched = fetched;
    incrementPC(state);
  }
  execute(state); // Execute HAL instruction
  free(state->pipeline.decoded);
  free(state);
  return EXIT_SUCCESS;
}
