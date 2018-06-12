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
  }

  if(!strcmp(src, "asr") || !strcmp(src, "lsl")
  || !strcmp(src, "lsr") || !strcmp(src, "ror")){
    return T_SHIFT;
  }
  return T_STR;
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
  if(len == 0){
    return 0;
  }
  // Loops through src to count number of tokens needed
  int splits = 0;
  int noSpaces = 0;
  for(int i = 0; i < len; i++){
    if(strchr(tokens, src[i])){
      splits++;
      if((i+1 < len) && src[i+1]!=sep && !strchr(tokens, src[i+1])){ //
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
  if(*output == NULL){
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

  for (int j = 0; j < mem_size-1; j++) {
    if(mem[j]=='\0'){
      currentpart++;
      *currentpart = &mem[j+1];
    }
  }
  // free(mem); TODO: FREE THIS SOMEWHERE
  return n;
}

token_t *token_new(token_type_t type, char *str){
  token_t *tkn = malloc(sizeof(token_t));
  if(tkn == NULL){
    return NULL;
  }
  tkn->type = type;
  tkn->str = str;
  return tkn;
}

void token_free(void *obj){
  token_t *tkn = (token_t *)obj;
  if(tkn){
    free(tkn->str);
  }
  free(tkn);
}

list_t *token_list_new(void){
  return list_new(&token_free);
}
void token_list_destroy(list_t *self){
  list_destroy(self);
}
int token_list_add(list_t *self, token_t *token){
  return list_add(self, token);
}
int token_list_add_pair(list_t *self, token_type_t type, char *str){
  return token_list_add(self, token_new(type, str));
}
token_t *token_list_get(list_t *self, int idx){
  return (token_t *)list_get(self, idx);
}
token_type_t token_list_get_type(list_t *self, int idx){
  return token_list_get(self, idx)->type;
}
char *token_list_get_str(list_t *self, int idx){
  return token_list_get(self, idx)->str;
}
int token_list_remove(list_t *self, int idx){
  return list_remove(self, idx);
}

void print_token_lst(list_t *tklst){
  printf("Tokens (%u)\n", tklst->len);
  for (int i = 0; i < tklst->len; i++) {
    printf("%u(T:%u): %s\n", i, token_list_get_type(tklst, i), token_list_get_str(tklst, i));
  }
}

int tokenize(char *line, list_t **tkns){
  assert(tkns != NULL);
  DEBUG_PRINT("Tokenize started on line: %s", line);
  char **token_strs = NULL;

  line[strcspn(line, "\r\n")] = 0; //Ends new line

  int n = str_separate(line, "[],:", ' ', &token_strs);
  DEBUG_PRINT("Number of tokens:%d\n", n);
  if(n==0){
    return EC_SKIP;
  }
  *tkns = list_new(&token_free);
  if(*tkns == NULL){
    return EC_NULL_POINTER;
  }

  list_add(*tkns, token_new(T_OPCODE, token_strs[0]));

  for (int i = 1; i < n; i++) {
    list_add(*tkns, token_new(token_type(token_strs[i]), token_strs[i]));
  }
  free(token_strs);

  DEBUG_CMD(print_token_lst(*tkns));
  return EC_OK;
}
