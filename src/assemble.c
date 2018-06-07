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
program_t *program_new() {
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
  program->in = allocate_input(MAX_NUM_LINES, LINE_SIZE);
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
 * Print out the string representation of the program
 *
 * @param out : Current binary stored in memory
 * @param lines : Number of lines to print
 */
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

  program_t *program = program_new();

  if (program == NULL) {
    return EC_SYS; // unable to allocate space for program.
  }

  if (read_char_file(argv[1], program->in,  &program->lines)) {
    return EC_SYS; // failed to read input. Need this in emulate too?
  }

  // set up variables for assembler
  token_list_t *lineTokens;
  int numTokens;
  instruction_t instr;
  word_t word;

  //convert each line to binary
  for (int i = 0; i < program->lines; ++i) {

    tokenize(program->in[i], &lineTokens);

    if (parse(program, lineTokens, &instr)) {
      free(lineTokens);
      return EC_SYS; //parse failed
    }
    free(&lineTokens);

    if (encode(&instr, &word)) {
      return EC_SYS; // encode failed
    }

    program->out[i * 4] = word;
    if (word == 0) {
      continue;
    }
    program->mPC += 4;
  }

  write_file(argv[2], program->out, sizeof(program->out));

  program_delete(program);

  return EC_OK;
}
