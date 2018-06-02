#include <stdlib.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/io.h"
#include "assemble.h"

/**
 * Allocate memory for the program.
 *
 * @return : a pointer to an uninitialised program.
 */
program_t *program_new(void) {
  program_t *program = malloc(sizeof(program_t));
  assert(program != NULL);
  return &program;
}

/**
 * Free program memory
 *
 * @param program : the desired program to remove from memory.
 * @return : free will always succeed so returns 0.
 */
int program_delete(program_t *program) {
  free(program);
  return 0;
}

/**
 *
 * @param program
 * @param label
 * @param addr
 * @return
 */
int program_add_symbol(program_t *program, label_t label, address_t addr) {
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
    binInstr = program->mPC;
    if (binInstr == 0) { // halt when memory instr is 0.
      continue;
    }
    printf("0x%08x: 0b%032b\n", wordAddr, binInstr);
  }
}


int main(int argc, char **argv) {
  assert(argc > 1);
}


