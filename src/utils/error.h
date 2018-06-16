/*
 * Contains the definitons of error codes and structure of an error.
 * Uses preprocessor directives to accomplish this.
 */

#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  EC_OK,
  EC_INVALID_PARAM,
  EC_NULL_POINTER,
  EC_UNSUPPORTED_OP,
  EC_SKIP,
  EC_IS_LABEL,
  EC_SYS, // Must be last
} error_code;

typedef struct {
  int code;
  const char *message;
} error_str;

// Convert error codes to and from system ones
#define EC_FROM_SYS_ERROR(e) (EC_SYS + e)
#define EC_TO_SYS_ERROR(e) (e - EC_SYS)
#define EC_IS_SYS_ERROR(e) (e >= EC_SYS)

/*  Usage:
 *
 *  Start of function should include int _status = EC_OK.
 *  Labelled area called fail: to jump to in case of error.
 *  return _status.
 */

//DO-While allows macro to resemble single statement
#define FAIL_PRED(pred, status) \
  do {if (pred) {return status;}} while(0)
#define FAIL_SYS(pred) \
  do {if (pred) {return EC_FROM_SYS_ERROR(errno);}} while(0)
#define FAIL_FORWARD(expr) \
  do {_status = expr; if (_status != EC_OK) return _status} while(0)

#define MEM_CHECK_C(expr, ret, cleanup) \
  do { \
    if(expr == NULL){ \
      fprintf(stderr, "%s:%d:%s(): allocate failed\n", __FILE__, __LINE__, __func__); \
      cleanup; \
      return ret; \
    }} while(0)

#define MEM_CHECK(expr, ret) \
  do { \
    if(expr == NULL){ \
      fprintf(stderr, "%s:%d:%s(): allocate failed\n", __FILE__, __LINE__, __func__); \
      return ret; \
    }} while(0)

#define CHECK_STATUS(status, cleanup) \
  do {if(status != EC_OK){ \
    ec_strerror(stderr, status, __FILE__, __LINE__, __func__); \
    cleanup; \
    return status; \
  }} while(0)
void ec_strerror(FILE *out, const int status, char *file, int line,
                 const char *func);

#endif
