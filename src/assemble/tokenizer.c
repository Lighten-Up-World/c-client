#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tokenizer.h"
#include "../utils/error.h"

/**
* Determins token type by first character for all types except opcode
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
    case 'r':
      return T_REGISTER;
    default:
      return T_SHIFTNAME;
  }
}

/**
* Takes a source string, one character separator and a pointer to an external
* character separator used for returning sub strings.
*
* @param src: the source string. Assumes NULL terminated string
* @param sep: the char separator
* @param ouput: the place in memory to store the separated sublists
* @returns The number of tokens found
*/
int str_separate(char *src, char *tokens, char sep, char ***output){
  assert(src != NULL && output != NULL);

  int len = strlen(src);



  int n = 0;
  int lastString = 0;
  for(int i = 0; i < len; i++){
    if(src[i] == sep){
      if(lastString){
        n++;
      }
      lastString = 0;
    }
    else if(strchr(tokens, src[i])){
      n++;
      if(lastString){
        n++;
      }
      lastString = 0;
    }
    else{
      lastString++;
    }
  }
  *output = malloc(n * sizeof(char *));
  char **currentpart = *output;
  int mem_size = len + n - 1;
  char *mem = calloc(mem_size, sizeof(char));
  *currentpart = mem;

  if(currentpart == NULL){
    return 1;
  }
  int j = 0;
  for (int i = 0; i < len; i++) {
    if(src[i]==sep){ //Sep
      j++;
      continue;
    }
    if(strchr(tokens,src[i])){ //Token
      mem[j] = src[i];
      j++;
    }
    else{ //Character
      mem[j] = src[i];
      if(strchr(tokens,src[i+1])){
        j++;
      }
    }
    j++;
  }

  //TODO: Merge loops together.
  for (int j = 0; j < mem_size; j++) {
    if(mem[j]=='\0'){
      currentpart++;
      *currentpart = &mem[j+1];
    }
  }

  return n;
}

int tokenize(char *line, token_t **out){

  char **token_strs = NULL;
  int n = str_separate(line, "[,]", ' ', &token_strs);
  *out = malloc(n * sizeof(token_t));
  if(*out == NULL){
    return -1;
  }
  **out = (token_t) {T_OPCODE, token_strs[0]};
  for (int i = 1; i < n; i++) {
    *(*out+i) = (token_t) {token_type(token_strs[i]), token_strs[i]};
  }
  free(token_strs);
  return n;
}

// int main(int argc, char const *argv[]) {
//   char str[] = "str r3,[r2,#3]";
//   token_t *out = NULL;
//   printf("%p\n", (void *) &out);
//   int n = tokenize(str, &out);
//   printf("%p\n", (void *) out);
//   for (int i = 0; i < n; i++) {
//     printf("%s\n", out[i].str);
//   }
//   free(out[0].str);
//   free(out);
//   return 0;
// }
