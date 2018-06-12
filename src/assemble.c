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
  if(in == NULL){
    return NULL;
  }
  in[0] = malloc(lines * lineLength * sizeof(char));
  if (in[0] == NULL) {
    free(in);
    return NULL; // failed;
  }

  for (i = 1; i < lines; i++) {
    in[i] = in[0] + i * lineLength * sizeof(char) + 1;
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
  if (program == NULL) {
    return NULL;
  }

  program->sym_m = smap_new(MAX_S_MAP_CAPACITY);
  if (program->sym_m == NULL) {
    free(program);
    return NULL;
  }
  program->ref_m = rmap_new(MAX_R_MAP_CAPACITY);
  if (program->ref_m == NULL) {
    smap_delete(program->sym_m);
    free(program);
    return NULL;
  }

  // Ideally use function to count the number of lines
  program->in = allocate_input(MAX_NUM_LINES, LINE_SIZE);
  if (program->in == NULL) {
    rmap_delete(program->ref_m);
    smap_delete(program->sym_m);
    free(program);
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
  int _status = EC_OK;
  assert(argc > 2);

  program_t *program = program_new();

  if (program == NULL) {
    return EC_NULL_POINTER; // unable to allocate space for program.
  }
  DEBUG_PRINT("Starting read of file @%s\n", argv[1]);
  if ((_status = read_char_file(argv[1], program->in,  &program->lines))) {
    DEBUG_PRINT("%u\n", _status);
    program_delete(program);
    return _status; // failed to read input. Need this in emulate too?
  }
  DEBUG_PRINT("%u\n", _status);

  // set up variables for assembler
  token_list_t lineTokens;
  instruction_t instr;
  word_t word;

  //convert each line to binary
  for (int i = 0; i < program->lines; i++) {
    DEBUG_PRINT("======== LINE %u ======\n", i);
    int t_ec = tokenize(program->in[i], &lineTokens);
    if(t_ec == -1){
      return EC_NULL_POINTER;
    }
    if (parse(program, &lineTokens, &instr)) {
      program_delete(program);
      return EC_SYS; //parse failed
    }
    free(lineTokens.tkns);
    if (encode(&instr, &word)) {
      program_delete(program);
      return EC_SYS; // encode failed
    }
    DEBUG_PRINT("Word is: %08x\n", word);
    set_word(program->out, program->mPC, word);
    if (word != 0) {
      program->mPC += 4;
    }
  }
  byte_t buff[program->mPC];
  for (size_t i = 0; i < program->mPC; i++) {
    buff[i] = program->out[i];
  }
  write_file(argv[2], buff, sizeof(buff));

  program_delete(program);

  return EC_OK;
}
