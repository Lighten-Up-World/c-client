#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAX_NUM_TOKENS

typedef enum{
  T_OPCODE,
  T_REGISTER,
  T_HASH_EXPR,
  T_EQ_EXPR,
  T_SHIFTNAME,
  T_L_BRACKET,
  T_R_BRACKET,
  T_COMMA,
  T_MINUS,
  T_PLUS,
}token_type_t;

typedef struct {
  token_type_t type;
  char *str;
} token_t;

/**
* Takes a source string, one character separator and a pointer to an external
* character separator used for returning sub strings.
*
* @param src: the source string
* @param sep: the char separator
* @param ouput: the place in memory to store the separated sublists
* @returns The number of tokens found
*/
int str_separate(char *src, char sep, char ***output);

int tokenize(char *line, token_t *out);

#endif
