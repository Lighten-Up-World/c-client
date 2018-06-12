#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "../utils/list.h"

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

int str_separate(char *src, char *tokens, char sep, char ***output);

// List Construction
list_t *token_list_new(void);
void token_list_delete(list_t *self);

int token_list_add(list_t *self, token_t *token);
int token_list_add_pair(list_t *self, token_type_t type, char *str);
token_t *token_list_get(list_t *self, int idx);
token_type_t token_list_get_type(list_t *self, int idx);
char *token_list_get_str(list_t *self, int idx);
int token_list_remove(list_t *self, int idx);

int tokenize(char *line, list_t **tkns);

#endif
