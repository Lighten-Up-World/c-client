/*
 * Initialises the memory and registers and loops through the instructions,
 * executing them until the halt instruction or end of memory is reached.
 */

#include "emulate.h"

/**
 * Allocate space on the heap for the emulator state
 *
 * @return: heap-allocated emulate_state_t struct
 */
static emulate_state_t *emulate_state_new() {
  emulate_state_t *state = calloc(1, sizeof(emulate_state_t));
  MEM_CHECK(state, NULL);
  state->pipeline.decoded = calloc(1, sizeof(instruction_t));
  MEM_CHECK(state->pipeline.decoded, NULL);
  state->pipeline.fetched = calloc(1, sizeof(word_t));
  MEM_CHECK(state->pipeline.fetched, NULL);

  return state;
}

/**
 * Free space on heap of an emulate_state_t
 *
 * @param state: pointer to the emulate_state_t to free
 */
static void emulate_state_free(emulate_state_t *state) {
  if (state) {
    free(state->pipeline.decoded);
    free(state->pipeline.fetched);
  }
  free(state);
}

/**
 * Main Emulator Loop
 *
 * Takes as argument the name of the binary file to emulate
 */
int main(int argc, char **argv) {
  int _status = EC_OK;
  if (argc <= 1) {
    fprintf(stderr, "%s <path_to_binary_file>", argv[0]);
    return EC_INVALID_PARAM;
  }

  emulate_state_t *state = emulate_state_new();
  MEM_CHECK_C(state, EC_NULL_POINTER, emulate_state_free(state));

  _status = read_file(argv[1], state->memory, MEM_SIZE);
  CHECK_STATUS(_status, emulate_state_free(state));

  //Setup Pipeline
  set_pc(state, 0x8);
  _status = get_mem_word(state, 0, state->pipeline.fetched);
  CHECK_STATUS(_status, emulate_state_free(state));

  _status = decode_word(state->pipeline.decoded, *state->pipeline.fetched);
  get_mem_word(state, 0x4, state->pipeline.fetched);

  while (state->pipeline.decoded->type != HAL) {
    _status = execute(state);
    if (!_status) {
      _status = decode_word(state->pipeline.decoded, *state->pipeline.fetched);
      get_mem_word(state, get_pc(state), state->pipeline.fetched);
    }
    increment_pc(state);
  }

  _status = execute(state);
  emulate_state_free(state);
  return _status;
}
