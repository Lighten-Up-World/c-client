#include <stdlib.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/io.h"
#include "symbolmap.h"
#include "referencemap.h"
#include "assemble.h"

/**
 *
 * @param lines : number of lines of the input file
 * @param lineLength : standard length of input file lines
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
 * @return : a pointer to an uninitialised program.
 */
program_t *program_new(void) {
  program_t *program;
  program = malloc(sizeof(program_t));
  if (!program) {
    //ERROR
  }

  program->sym_m = smap_new(/* some capacity */);
  program->ref_m = rmap_new(/* some capacity */);

  program->in = allocate_input(/* num lines */, /* line length */);

  return program;
}

/**
 * Free program memory
 *
 * @param program : the desired program to remove from memory.
 * @return : free will always succeed so returns 0.
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
 * @param program
 * @param label
 * @param addr
 * @return
 */
int program_add_symbol(program_t *program, label_t label, address_t addr) {
  // add symbol to symbol map in program.
  if (!smap_put(program->sym_m, label, addr)) {
    return 0;
  }

  // TODO: check refmap to see if symbol appears and remove/update references
  return 1;
}

/**
 *
 * @param program
 * @param label
 * @param addr
 * @return
 */
int program_add_reference(program_t *program, label_t label, address_t addr) {
  // adds reference to ref_map.
  if (!rmap_put(program->ref_m, label, addr)) {
    return 0;
  }
  return 1;
}

/**
 *
 * @param program : a pointer to the program information DataType
 * @param string : NO IDEA WHAT THIS IS FOR
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
    printf("%08x: %08  ....", wordAddr, binInstr); // what are the .s all about?
  }
}


int main(int argc, char **argv) {
  assert(argc > 1);
}


