#include "../../unity/src/unity.h"
#include "../utils/io.h"
#include <stdlib.h>

void test_read_char_file(void){
  char path[] = "../../unit_tests/test_cases/add01.s";
  char **buff = malloc(100 * sizeof(char *));
  for (size_t i = 0; i < 100; i++) {
    buff[i] = malloc(512 * sizeof(char));
  }
  int num_of_lines = read_char_file(path, buff);
  for (size_t i = 0; i < 100; i++) {
    for (size_t j = 0; j < 512; j++) {
      printf("%c", buff[i][j]);
    }
  }
  printf("%s\n", path);
  printf("%u\n", num_of_lines);
}

void test_read_file(void){
  char path[] = "../../unit_tests/test_cases/add01.s";
  char *buff = malloc(100 * sizeof(char));
  read_file(path, buff, 100 * sizeof(char));
  for (size_t i = 0; i < 100; i++) {
      printf("%c", buff[i]);
  }
  printf("\n");
  printf("%s\n", path);
}
