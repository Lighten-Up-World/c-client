#include <stdlib.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/io.h"
#include "assemble/tokenizer.h"
#include "assemble/symbolmap.h"
#include "assemble/referencemap.h"
#include "assemble.h"

/**
 *
 * @param lines : number of lines of the input file
 * @param lineLength : length of input file lines
 * @return : pointer to the array containing input, with memory allocated but
 * not initialised.
 */
char **allocate_input(int lines, int lineLength) {
  char **in;
  unsigned int i;

  in = malloc(lines * sizeof(char *));
  // ^^ do I need to cast here even though it's implicit?
  in[0] = malloc(lines * lineLength * sizeof(char));
  if (!in[0]) {
    return NULL; // failed;
  }

  for (i = 0; i < lines; i++) {
    in[i] = in[i-1] + lineLength;
  }

  return in;
}

/**
 * Allocate memory for the program.
 *
 * @return : pointer to an uninitialised program.
 */
program_t *program_new(void) {
  program_t *program = NULL;
  program = malloc(sizeof(program_t));
  if (!program) {
    //ERROR : set program to null?
  }

  program->sym_m = smap_new(MAX_S_MAP_CAPACITY);
  program->ref_m = rmap_new(MAX_R_MAP_CAPACITY);

  program->in = allocate_input(MAX_NUM_LINES, MAX_LINE_LENGTH);
  if (program->in == NULL) {
    //ERROR : unable to allocate input
  }

  return program;
}

/**
 * Free program memory
 *
 * @param program : desired program to remove from memory.
 * @return : free will always succeed so returns 1.
 */
int program_delete(program_t *program) {
  // free input characters
  free(program->in[0]);
  free(program->in);
  // free data structures
  smap_delete(program->sym_m);
  rmap_delete(program->ref_m);
  // free rest of program
  free(program);
  return 1;
}

/**
 *
 * @param program : pointer to the program information DataType
 * @param label
 * @param addr
 * @return
 */
int program_add_symbol(program_t *program, label_t label, address_t addr) {
  if (!smap_put(program->sym_m, label, addr)) {
    return 0; // already in symbol map
  }

  address_t *outReferences = calloc();
  int outsize;
  // check if symbol exists in ref_map and update/remove accordingly
  if (rmap_exists(program->ref_m, label)) {
    if (rmap_get_references(program->ref_m, label, outReferences, outsize)) {
      //TODO : check if addr is  in the list of references already.
    }
  }
  rmap_put(program->ref_m, label, addr);

  return 1;
}

/**
 *
 * @param program : pointer to the program information DataType
 * @param label
 * @param addr
 * @return
 */
int program_add_reference(program_t *program, label_t label, address_t addr) {
  // adds reference to ref_map.
  return !rmap_put(program->ref_m, label, addr);
}

/**
 *
 * @param program : pointer to the program information DataType
 * @param string : output string representation of the program
 */
void program_toString(program_t *program, char *string) {
  assert(program != NULL);
  assert(string != NULL);
  word_t binInstr;

  for (int wordAddr = 0; wordAddr < program->lines; wordAddr += 4) {
    binInstr = program->out[wordAddr];// do I need a getMemWord style func here?
    if (binInstr == 0) { // halt when memory instr is 0.
      continue;
    }
    // add word addr and bininstr to string, memcpy or strcat?
  }
}


int main(int argc, char **argv) {
  assert(argc > 1);

  program_t *program = program_new();
  if (read_file(argv[1], program->in, sizeof(program->in))) {
    return 0; // failed to read input. Need this in emulate too?
  }
  char ***out;
  program->lines = str_separate(program->in,"", "\n", out);
}


