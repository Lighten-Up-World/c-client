#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../utils/arm.h"//EC
#include "tokenizer.h"
#include "../utils/error.h"

/**
* Determines token type by first character for all types except opcode
*/
int token_type(char *src){
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
    default:
      return T_SHIFT;
  }
}


/**
*
* @param src: the source string. Assumes NULL terminated string
* @param sep: the char separator
* @param ouput: the place in memory to store the separated sublists
* @returns The number of tokens found
*/
int str_separate(char *src, char *tokens, char sep, char ***output){
  assert(src != NULL && output != NULL);

  int len = strlen(src);

  // Loops through src to count number of tokens needed
  int splits = 0;
  int noSpaces = 0;
  for(int i = 0; i < len; i++){
    if(strchr(tokens, src[i])){
      splits++;
      if((i+1 < len) && !(src[i+1]==sep) && !strchr(tokens, src[i+1])){ //
        splits++;
      }
    }
    if(src[i] == sep){
      noSpaces++;
      if((i+1 < len) && src[i+1] != sep && !strchr(tokens, src[i+1]) ){
        splits++;
      }
    }
  }
  int n = splits + 1;
  *output = malloc(n * sizeof(char *)); // Allocated memory for 2D array outer
  if(output == NULL){
    return -EC_NULL_POINTER;
  }
  char **currentpart = *output;
  int mem_size = len + n - noSpaces;
  char *mem = calloc(mem_size, sizeof(char)); // Allocate memory for inner array
  if(mem == NULL){
    return -EC_NULL_POINTER;
  }

  // Loop through source and place in correct position in mem
  int j = 0;
  for (int i = 0; i < len; i++) {
    if(src[i]==sep){ //Sep
      if(src[i-1]!=sep){
        j++;
      }
      continue;
    }
    mem[j] = src[i];
    j++;
    if(strchr(tokens,src[i]) && src[i+1]!=sep){ //Token
      j++;
    }
    else if(strchr(tokens,src[i+1])){
      j++;
    }

  }
  // Allocated outer pointers to correct position in memory
  *currentpart = mem;
  for (int j = 0; j < mem_size; j++) {
    if(mem[j]=='\0'){
      currentpart++;
      *currentpart = &mem[j+1];
    }
  }

  return n;
}

void print_token_lst(token_list_t tklst){
  printf("Tokens (%u)\n", tklst.numOfTkns);
  for (size_t i = 0; i < tklst.numOfTkns; i++) {
    printf("%u: %s\n", tklst.tkns[i].type, tklst.tkns[i].str);
  }
}

int tokenize(char *line, token_list_t *out){
  DEBUG_PRINT("Tokenize started on line: %s", line);
  char **token_strs = NULL;

  line[strlen(line) - 1] = '\0'; //Strips \n

  int n = str_separate(line, "[],:", ' ', &token_strs);
  token_t *tkns = malloc(n * sizeof(token_t));
  if(tkns == NULL){
    return -1;
  }

  *tkns = (token_t) {T_OPCODE, token_strs[0]};
  for (int i = 1; i < n; i++) {
    *(tkns+i) = (token_t) {token_type(token_strs[i]), token_strs[i]};
  }
  free(token_strs);

  *out = (token_list_t) {tkns, n};
  DEBUG_CMD(print_token_lst(*out));
  return n;
}
