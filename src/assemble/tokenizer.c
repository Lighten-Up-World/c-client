/*
 * Contains function tokenize which takes in a program line and stores an array
 * of substrings (tokens) such as ["mov', "r2" ,",", "\#1"] for parsing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../utils/arm.h"//EC
#include "tokenizer.h"
#include "../utils/error.h"

/**
 * Determine token type by first character for all types except opcode
 *
 * @param src: source string
 * @return: integer associated with token type enum value
 */
int token_type(char *src) {
  switch (src[0]) {
    case '+':
      return T_PLUS;
    case '-':
      return T_MINUS;
    case ',':
      return T_COMMA;
    case '[':
      return T_L_BRACKET;
    case ']':
      return T_R_BRACKET;
    case '=':
      return T_EQ_EXPR;
    case '#':
      return T_HASH_EXPR;
    case ':':
      return T_LABEL;
    case 'r':
      return T_REGISTER;
  }

  if (!strcmp(src, "asr") || !strcmp(src, "lsl")
      || !strcmp(src, "lsr") || !strcmp(src, "ror")) {
    return T_SHIFT;
  }
  return T_STR;
}


/**
 * Skip separator characters and build a list of strings which are tokens
 * (for use in tokenize)
 *
 * @param src: source string. Assumes NULL terminated string
 * @param sep: char separator
 * @param ouput: location in memory to store the separated sublists
 * @return: number of tokens found
 */
int str_separate(char *src, char *tokens, char sep, char ***output) {
  assert(src != NULL && output != NULL);

  int len = strlen(src);
  if (len == 0) {
    return 0;
  }
  // Loops through src to count number of tokens needed
  int splits = 0;
  int noSpaces = 0;
  for (int i = 0; i < len; i++) {
    if (strchr(tokens, src[i])) {
      splits++;
      if ((i + 1 < len) && src[i + 1] != sep
          && !strchr(tokens, src[i + 1])) { //
        splits++;
      }
    }
    if (src[i] == sep) {
      noSpaces++;
      if ((i + 1 < len) && src[i + 1] != sep && !strchr(tokens, src[i + 1])) {
        splits++;
      }
    }
  }
  int n = splits + 1;
  *output = malloc(n * sizeof(char *)); // Allocated memory for 2D array outer
  if (*output == NULL) {
    return -EC_NULL_POINTER;
  }
  char **currentpart = *output;
  int mem_size = len + n - noSpaces;
  char *mem = calloc(mem_size, sizeof(char)); // Allocate memory for inner array
  if (mem == NULL) {
    return -EC_NULL_POINTER;
  }

  // Loop through source and place in correct position in mem
  int j = 0;
  for (int i = 0; i < len; i++) {
    if (src[i] == sep) { //Sep
      if (src[i - 1] != sep) {
        j++;
      }
      continue;
    }
    mem[j] = src[i];
    j++;
    if (strchr(tokens, src[i]) && src[i + 1] != sep) { //Token
      j++;
    } else if (strchr(tokens, src[i + 1])) {
      j++;
    }
  }

  // Allocated outer pointers to correct position in memory
  *currentpart = mem;

  for (int j = 0; j < mem_size - 1; j++) {
    if (mem[j] == '\0') {
      currentpart++;
      *currentpart = &mem[j + 1];
    }
  }
  return n;
}

//// Create and Remove Tokens in Memory ////

token_t *token_new(token_type_t type, char *str) {
  token_t *tkn = malloc(sizeof(token_t));
  if (tkn == NULL) {
    return NULL;
  }
  tkn->type = type;
  tkn->str = strdup(str);
  return tkn;
}

void token_free(void *obj) {
  token_t *tkn = (token_t *) obj;
  if (tkn) {
    free(tkn->str);
  }
  free(tkn);
}

//// Token List Utility Functions ////

list_t *token_list_new(void) {
  return list_new(&token_free);
}

void token_list_delete(list_t *self) {
  list_delete(self);
}

int token_list_add(list_t *self, token_t *token) {
  return token_list_add_pair(self, token->type, token->str);
}

int token_list_add_pair(list_t *self, token_type_t type, char *str) {
  return list_add(self, token_new(type, str));
}

token_t *token_list_get(list_t *self, int idx) {
  return (token_t *) list_get(self, idx);
}

token_type_t token_list_get_type(list_t *self, int idx) {
  return token_list_get(self, idx)->type;
}

char *token_list_get_str(list_t *self, int idx) {
  return token_list_get(self, idx)->str;
}

int token_list_remove(list_t *self, int idx) {
  return list_remove(self, idx);
}

void token_list_print(list_t *tklst) {
  printf("Tokens (%u)\n", tklst->len);
  for (int i = 0; i < tklst->len; i++) {
    printf("%u(T:%u): %s\n",
           i,
           token_list_get_type(tklst, i),
           token_list_get_str(tklst, i));
  }
}

/**
 * Use str_separate to build a list of tokens in the current line
 *
 * @param line: current line of assembly
 * @param tkns: empty list of tokens
 * @return: integer error code based on success of function
 */
int tokenize(char *line, list_t **tkns) {
  assert(tkns != NULL);

  char **token_strs = NULL;

  line[strcspn(line, "\r\n")] = 0; //Ends new line

  int n = str_separate(line, "[],:", ' ', &token_strs);
  if (n == 0) {
    return EC_SKIP;
  }
  if (*tkns != NULL) {
    token_list_delete(*tkns);
  }
  *tkns = token_list_new();
  if (*tkns == NULL) {
    return EC_NULL_POINTER;
  }

  token_list_add_pair(*tkns, T_OPCODE, token_strs[0]);

  for (int i = 1; i < n; i++) {
    token_list_add_pair(*tkns, token_type(token_strs[i]), token_strs[i]);
  }
  free(token_strs[0]);
  free(token_strs);

  return EC_OK;
}
