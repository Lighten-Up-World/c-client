#include <stdlib.h>
#include <assert.h>
#include "utils/arm.h"
#include "utils/io.h"
#include "utils/error.h"
#include "assemble/symbolmap.h"
#include "assemble/referencemap.h"
#include "assemble/tokenizer.h"
#include "assemble/parser.h"
#include "assemble/encode.h"
#include "assemble.h"

/**
 * Allocate memory for the input stored in the program
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
  program_t *program;
  program = malloc(sizeof(program_t));
  if (!program) {
    return NULL;
  }

  program->sym_m = smap_new(MAX_S_MAP_CAPACITY);
  program->ref_m = rmap_new(MAX_R_MAP_CAPACITY);
  if (program->sym_m == NULL) {
    return NULL;
  }
  if (program->ref_m == NULL) {
    return NULL;
  }

  // Ideally use function to count the number of lines
  program->in = allocate_input(MAX_NUM_LINES, MAX_LINE_LENGTH);
  if (program->in == NULL) {
    return NULL;
  }

  return program;
}

/**
 * Free program memory
 *
 * @param program : desired program to remove from memory.
 * @return : free will always succeed so returns EC_OK.
 */
int program_delete(program_t *program) {
  // free input characters
  free(program->in[0]);
  free(program->in);
  // free data structures
  rmap_delete(program->ref_m);
  smap_delete(program->sym_m);
  // free rest of program
  free(program);

  return EC_OK;
}

/**
 * Add a symbol to the symbol table in the program and update reference table
 *
 * @param program : pointer to the program information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int program_add_symbol(program_t *program, label_t label, address_t addr) {
  if (!smap_put(program->sym_m, label, addr)) {
    return 0; // already in symbol map
  }

  // check if symbol exists in ref_map and update/remove accordingly
  if (rmap_exists(program->ref_m, label)) {
    //TODO : check if addr is  in the list of references already.
  }

  return 1;
}

/**
 * add a symbol to the reference map stored in the program
 *
 * @param program : pointer to the program information DataType
 * @param label : represents a point to branch off to
 * @param addr : address accompanied by label
 * @return : 0 or 1 depending whether the addition was successful or not
 */
int program_add_reference(program_t *program, label_t label, address_t addr) {
  // adds reference to ref_map.
  return !rmap_put(program->ref_m, label, addr);
}

/**
 * Create the string representation of the program
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
    // add word addr and binInstr to string, memcpy or strcat?
  }
}


// main assembly loop.
int main(int argc, char **argv) {
  assert(argc > 2);

  program_t *program = program_new();
  // readfile takes a byte* and program->in is a char**
  if (read_file(argv[1], program->in, sizeof(program->in))) {
    return 0; // failed to read input. Need this in emulate too?
  }
  char **out;
  program->lines = str_separate(program->in,"", "\n", &out);
  free(out); // maybe not

  // set up variables for assembler
  token_t *lineTokens;
  lineTokens = malloc(MAX_NUM_TOKENS * sizeof(token_t));

  instruction_t *instr;
  instr = malloc(sizeof(instruction_t));

  word_t *word;

  //convert each line to binary
  for (int i = 0; i < program->lines; ++i) {
    tokenize(program->in[i], lineTokens);
    parse(lineTokens, instr);
    encode(instr, word);

    program->out[i * 4] = word;
    if (word == 0) {
      continue;
    }
    instr = NULL; //probably wrong
    lineTokens = NULL; // probably wrong
  }

  char **stringRep = allocate_input(program->lines, MAX_LINE_LENGTH);

  //

  free(word);
  free(instr);
  free(lineTokens);

  program_delete(program);
}


