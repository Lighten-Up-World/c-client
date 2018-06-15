/*
 * Contains the main function which reads in the lines of the program. Then,
 * each line is tokenized, parsed and encoded into binary words. The binary
 * words are written to an out file.
 */

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
 * @param lines: number of lines of the input file
 * @param lineLength: length of input file lines
 * @return: pointer to the array containing input, with memory allocated but
 * not initialised
 */
char **allocate_input(int lines, int lineLength) {
  char **in = malloc(lines * sizeof(char *));
  if (in == NULL) {
    return NULL;
  }
  in[0] = calloc(lines * lineLength, sizeof(char));
  if (in[0] == NULL) {
    free(in);
    return NULL; // failed;
  }

  for (int i = 1; i < lines; i++) {
    in[i] = in[0] + i * lineLength + 1;
  }

  return in;
}

/**
 * Free program_state memory
 *
 * @param program_state: desired program_state to remove from memory
 * @return: free will always succeed so returns EC_OK
 */
int program_delete(assemble_state_t *program) {
  if (program) {
    free(program->out);
    // free input characters
    if (program->in) {
      free(program->in[0]);
    }
    free(program->in);
    // free data structures
    rmap_delete(program->rmap);
    smap_delete(program->smap);
    list_delete(program->additional_words);
    list_delete(program->tklst);
  }
  // free rest of program
  free(program);

  return EC_OK;
}

/**
 * Allocate memory for the program
 *
 * @return: pointer to an uninitialised program
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
    program_delete(program);
    return NULL;
  }
  program->rmap = rmap_new(MAX_R_MAP_CAPACITY);
  if (program->rmap == NULL) {
    program_delete(program);
    return NULL;
  }
  program->additional_words = list_new(&free);
  if (program->additional_words == NULL) {
    program_delete(program);
    return NULL;
  }
  program->in = allocate_input(MAX_LINES, LINE_SIZE);
  if (program->in == NULL) {
    program_delete(program);
    return NULL;
  }

  program->tklst = token_list_new();
  if (program->tklst == NULL) {
    program_delete(program);
    return NULL;
  }

  program->out = calloc(sizeof(word_t), MAX_LINES * LINE_SIZE);

  if (program->smap == NULL) {
    program_delete(program);
    return NULL;
  }

  return program;
}

/**
 * Write the binary stored in the program struct to a file
 *
 * @param path: path to the binary file to write out to
 * @param program: pointer to the program state
 * @return: integer error code to represent success or failure
 */
int write_program(char *path, assemble_state_t *program) {
  int no_bytes = program->mPC + program->additional_words->len * 4;
  program->out = realloc(program->out, sizeof(byte_t) * no_bytes);
  if (program->out == NULL) {
    return EC_NULL_POINTER;
  }

  for (int i = 0; i < program->additional_words->len; i++) {
    wordref_t *wordref = list_get(program->additional_words, i);
    word_t offset = (program->mPC + i * 4 - wordref->ref - 8) | 0xFFFFF000;
    word_t referenced_word;
    get_word(program->out, wordref->ref, &referenced_word);
    referenced_word &= offset;
    set_word(program->out, wordref->ref, referenced_word);

    set_word(program->out, i * 4 + program->mPC, wordref->word);
  }

  return write_file(path, program->out, no_bytes);
}

/**
 * Main Assembler Loop
 *
 * Takes as arguments the .s file to assemble and the path name of the .bin
 * file to write out to.
 *
 */
int main(int argc, char **argv) {
  int _status = EC_OK;
  assert(argc > 2);

  assemble_state_t *program = program_new();

  if (program == NULL) {
    return EC_NULL_POINTER; // unable to allocate space for program.
  }
  program->lines = read_char_file(argv[1], program->in);

  // set up variables for assembler
  instruction_t instr;
  word_t word;

  //convert each line to binary
  for (int i = 0; i < program->lines; i++) {
    _status = tokenize(program->in[i], &program->tklst);
    if (_status == EC_SKIP) {
      _status = EC_OK;
      continue;
    }
    CHECK_STATUS(_status, program_delete(program));

    _status = parse(program, &instr);
    if (_status == EC_SKIP) {
      _status = EC_OK;
      continue;
    }
    CHECK_STATUS(_status, program_delete(program));

    _status = encode(&instr, &word);
    CHECK_STATUS(_status, program_delete(program));

    set_word(program->out, program->mPC, word);
    program->mPC += 4;
  }
  _status = write_program(argv[2], program);
  program_delete(program);

  return _status;
}
