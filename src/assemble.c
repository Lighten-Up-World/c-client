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

typedef struct {
  program_t *prog;
  label_t label;
  address_t addr;
} prog_collection_t;

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
program_t *program_new(int numLines) {
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
  program->in = allocate_input(numLines, LINE_SIZE);
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
* Update memory according to reference entry
*
* @param label : current string representation of the label in entry
* @param val : current value of entry
* @param obj : A prog_collection_t object which collects, program, label and addr.
*/
void ref_entry(const label_t label, const address_t val, const void *obj){
  prog_collection_t prog_coll = (prog_collection_t) obj;
  if(label == prog_coll.label){
    prog_coll.prog->out[val] = prog_coll.addr;
  }
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
  prog_collection_t prog_coll = {program, label, addr};
  // check if symbol exists in ref_map and update/remove accordingly
  if (rmap_exists(program->ref_m, label)) {
    rmap_enum(program->ref_m, ref_entry, prog_coll);
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
void program_toString(program_t *program) {
  assert(program != NULL);
  assert(string != NULL);
  word_t binInstr;

  for (int wordAddr = 0; wordAddr < program->lines; wordAddr += 4) {
    binInstr = program->out[wordAddr];// do I need a getMemWord style func here?
    if (binInstr == 0) { // halt when memory instr is 0.
      continue;
    }
    printf("%08x: %08x",wordAddr, binInstr);
  }
}

// To string for debugging
void print_bin_instr(byte_t *out, int lines) {
  int grouping = 2;
  int per_row = 8;

  int wordAddr;
  for (wordAddr = 0; wordAddr < lines; wordAddr += 1) {
    printf("%x", out[wordAddr]);
    if ((wordAddr + 1) % grouping == 0) {
      printf(" ");
    }
    if ((wordAddr + 1) % per_row == 0) {
      printf("\n");
    }
  }
  if ((wordAddr + 1) % 4 != 0) {
    printf("\n");
  }
}


// main assembly loop.
int main(int argc, char **argv) {
  assert(argc > 2);

  char fileContents[MAX_NUM_LINES * LINE_SIZE];
  // readfile takes a byte* and program->in is a char**
  if (read_file(argv[1], fileContents, sizeof(fileContents))) {
    return EC_SYS; // failed to read input. Need this in emulate too?
  }

  int numLines;
  char **out;
  numLines = str_separate(fileContents,"", '\n', &out);

  program_t *program = program_new(numLines);
  if (program == NULL) {
    return EC_SYS; // unable to allocate space for program.
  }
  program->lines = numLines;
  memcpy(program->in, out, sizeof(out));
  program->mPC = 0;

  free(out);

  // set up variables for assembler
  token_t *lineTokens;
  int numTokens;
  instruction_t instr;
  word_t word;

  //convert each line to binary
  for (int i = 0; i < numLines; ++i) {
    numTokens = tokenize(program->in[i], &lineTokens);
    parse(lineTokens, &instr, numTokens, program);
    encode(&instr, &word);

    program->out[i * 4] = word;
    if (word == 0) {
      continue;
    }
    program->mPC += 4;

    free(lineTokens);
  }

  write_file(argv[2], program->out, sizeof(program->out));

  program_delete(program);

  return EC_OK;
}


