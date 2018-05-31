#include <string.h>
#include <stdio.h>
#include "error.h"

void ec_strerror(const int status, char *buffer, const size_t buffer_length) {
  if (EC_IS_SYS_ERROR(status)) {
    strncpy(buffer, strerror(EC_TO_SYS_ERROR(status)), buffer_length);
    printf("%s\n", buffer);
  } else {
    size_t str_cnt = sizeof(error_strs) / sizeof(error_str);
    for (size_t i = 0; i < str_cnt; ++i) {
      if (error_strs[i].code == status) {
        strncpy(buffer, error_strs[i].message, buffer_length);
        return;
      }
    }
    strncpy(buffer, "An invalid/unhandled error code was encountered."
                    "Please report.", buffer_length);
  }
}
