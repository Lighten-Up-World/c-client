#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <stdio.h>

typedef struct csv_row {
    char **fields;
    int size;
} csv_row_t;

typedef struct csv_parser_t {
  char *path;
  char delimiter;
  char *errMsg;
  FILE *file;
	int from_string;
	char *csv_string;
	int csv_stringIter;
} csv_parser_t;


// Public
csv_parser_t *csv_parser_new(const char *filePath, const char *delimiter);
csv_parser_t *csv_parser_new_from_string(const char *csv_string, const char *delimiter);
void csv_parser_destroy(csv_parser_t *self);
void csv_parser_destroy_row(csv_row_t *row);
csv_row_t *csv_parser_getRow(csv_parser_t *self);
int csv_parser_getNumFields(csv_row_t *row);
char **csv_parser_getFields(csv_row_t *row);
const char* csv_parser_getErrorMessage(csv_parser_t *self);


#endif
