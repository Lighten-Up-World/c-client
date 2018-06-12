#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAX_NUM_TOKENS

typedef enum{
  T_OPCODE,
  T_REGISTER,
  T_HASH_EXPR,
  T_EQ_EXPR,
  T_SHIFT,
  T_L_BRACKET,
  T_R_BRACKET,
  T_COMMA,
  T_MINUS,
  T_PLUS,
  T_LABEL,
  T_STR,
} token_type_t;

typedef struct {
  token_type_t type;
  char *str;
} token_t;

typedef struct {
  token_t *tkns;
  int numOfTkns;
} token_list_t;

int str_separate(char *src, char *tokens, char sep, char ***output);

int tokenize(char *line, token_list_t *out);

#endif
