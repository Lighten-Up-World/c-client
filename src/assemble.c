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
  in[0] = calloc(lines * lineLength, sizeof(char));
  if (in[0] == NULL) {
    free(in);
    return NULL; // failed;
  }

  for (i = 1; i < lines; i++) {
    in[i] = in[0] + i * lineLength + 1;
  }

  return in;
}

/**
 * Allocate memory for the program.
 *
 * @return : pointer to an uninitialised program.
 */
assemble_state_t *program_new(void) {
  assemble_state_t *program;
  program = calloc(sizeof(assemble_state_t), 1);
  if (program == NULL) {
    return NULL;
  }

  program->lines = MAX_LINES;

  program->smap = smap_new(MAX_S_MAP_CAPACITY);
  if (program->smap == NULL) {
    free(program);
    return NULL;
  }
  program->rmap = rmap_new(MAX_R_MAP_CAPACITY);
  if (program->rmap == NULL) {
    smap_delete(program->smap);
    free(program);
    return NULL;
  }
  program->additional_words = list_new(&free);
  program->in = allocate_input(MAX_LINES, LINE_SIZE);
  if (program->in == NULL) {
    rmap_delete(program->rmap);
    smap_delete(program->smap);
    free(program);
    return NULL;
  }

  program->out = calloc(sizeof(word_t), MAX_LINES * LINE_SIZE);

  if (program->smap == NULL) {
    rmap_delete(program->rmap);
    smap_delete(program->smap);
    free(program);
    free(program->in[0]);
    free(program->in);
    return NULL;
  }

  return program;
}

/**
 * Free program_state memory
 *
 * @param program_state : desired program_state to remove from memory.
 * @return : free will always succeed so returns EC_OK.
 */
int program_delete(assemble_state_t *program) {
  free(program->out);
  // free input characters
  free(program->in[0]);
  free(program->in);
  // free data structures
  rmap_delete(program->rmap);
  smap_delete(program->smap);
  list_delete(program->additional_words);
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

int write_program(char *path, assemble_state_t *program){
  int no_bytes = program->mPC + program->additional_words->len * 4;
  program->out = realloc(program->out, sizeof(byte_t) * no_bytes);
  if(program->out == NULL){
      return EC_NULL_POINTER;
  }

  DEBUG_PRINT("Got %d additional words\n", program->additional_words->len );
  for (int i = 0; i < program->additional_words->len; i++) {
    wordref_t *wordref = list_get(program->additional_words, i);
    word_t offset = (program->mPC + i * 4 - wordref->ref - 8) | 0xFFFFF000;
    DEBUG_PRINT("offset calculated was: %08x\n", offset);
    word_t referenced_word;
    get_word(program->out, wordref->ref, &referenced_word);
    DEBUG_PRINT("Referenced Word was: %08x\n", referenced_word);
    referenced_word &= offset;
    set_word(program->out, wordref->ref, referenced_word);

    set_word(program->out, i * 4 + program->mPC, wordref->word);
  }
  write_file(path, program->out, no_bytes);

  return EC_OK;
}

// main assembly loop.
int main(int argc, char **argv) {
  int _status = EC_OK;
  assert(argc > 2);

  assemble_state_t *program = program_new();

  if (program == NULL) {
    return EC_NULL_POINTER; // unable to allocate space for program.
  }
  DEBUG_PRINT("Starting read of file @%s\n", argv[1]);
  program->lines = read_char_file(argv[1], program->in);

  // set up variables for assembler
  list_t *tklst = NULL;
  instruction_t instr;
  word_t word;

  //convert each line to binary
  for (int i = 0; i < program->lines; i++) {
    DEBUG_PRINT("\n======== LINE %u ======\n", i);
    _status = tokenize(program->in[i], &tklst);
    if(_status == EC_SKIP){
      _status = EC_OK;
      continue;
    }
    CHECK_STATUS(_status);

    _status = parse(program, tklst, &instr);
    if (_status == EC_SKIP) {
      _status = EC_OK;
      continue;
    }
    CHECK_STATUS_CLEANUP(_status, program_delete(program));

    //token_list_delete(tklst); TODO: Invalid frees atm
    _status = encode(&instr, &word);
    CHECK_STATUS_CLEANUP(_status, program_delete(program));

    DEBUG_PRINT("Word is: %08x\n", word);
    set_word(program->out, program->mPC, word);
    program->mPC += 4;
  }
  _status = write_program(argv[2], program);
  program_delete(program);

  return _status;
}
