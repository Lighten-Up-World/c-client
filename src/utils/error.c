/*
 * Contains the function used to throw errors
 * Interacts with the error.h file to convert system errors to error codes.
 */

#include <string.h>
#include "error.h"

/**
 * Convert an error code into an error message
 *
 * @param out: file stream to print error out to
 * @param status: error code that occurs
 * @param file: file name that error occurs in
 * @param line: line number error code occurs in
 * @param func: function to print that error occurs in
 */
void ec_strerror(FILE *out, const int status, char *file, int line,
                 const char *func) {
  const error_str error_strs[] = {
      {EC_OK, "No error encountered."},
      {EC_NULL_POINTER, "Passed a null pointer."},
      {EC_UNSUPPORTED_OP, "This operation is not supported."},
      {EC_INVALID_PARAM, "Invalid parameter."},
      {EC_SKIP, "Function wanting to skip remainer of loop."},
      {EC_IS_LABEL, "Special label error code."}
  };

  if (EC_IS_SYS_ERROR(status)) {
    fprintf(out, "%s:%d:%s(): %s\n", file, line, func,
            strerror(EC_TO_SYS_ERROR(status)));
  } else {
    size_t str_cnt = sizeof(error_strs) / sizeof(error_str);
    for (size_t i = 0; i < str_cnt; ++i) {
      if (error_strs[i].code == status) {
        fprintf(out, "%s:%d:%s(): %s\n", file, line, func,
                error_strs[i].message);
        return;
      }
    }
    fprintf(out, "%s:%d:%s(): An error was encountered with code %d\n",
            file, line, func, status);
  }
}
