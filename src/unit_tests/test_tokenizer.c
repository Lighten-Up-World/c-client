#include <stdlib.h>
#include <string.h>
#include "../../unity/src/unity.h"
#include "../assemble/tokenizer.h"

void string_separate_runner(char *str, char sep, char *tokens,
                            int expected_size, char **expected){
  char **token_strs = NULL;
  int n = str_separate(str, tokens, sep, &token_strs);
  for (int i = 0; i < n; i++) {
    printf("%s\n", token_strs[i]);
  }
  TEST_ASSERT_EQUAL_MESSAGE(expected_size, n, "Size of tokens differs");
  for (int i = 0; i < n; i++) {
    if(strcmp(token_strs[i], expected[i])){
      TEST_FAIL_MESSAGE("Token != Expected");
    }
  }
  free(token_strs);
}

void test_string_separate(void){
  char *short_expected[] = {"a", "b"};
  string_separate_runner("a  b", ' ', ",", 2, short_expected);

  char *str_expected[] = {"str", "r3", ",", "[", "r2", ",", "#", "3", "]"};
  string_separate_runner("str r3,[r2,#3]", ' ', "[],#", 9, str_expected);
//                        01234567890123456789012
  string_separate_runner("str  r3,[r2,#3]", ' ', "[],#", 9, str_expected);
  string_separate_runner("str r3 ,[r2,#3]", ' ', "[],#", 9, str_expected);
  string_separate_runner("str r3, [r2,#3]", ' ', "[],#", 9, str_expected);
  string_separate_runner("str r3,[ r2,#3]", ' ', "[],#", 9, str_expected);
  string_separate_runner("str r3,[r2 ,#3]", ' ', "[],#", 9, str_expected);
  string_separate_runner("str r3,[r2, #3]", ' ', "[],#", 9, str_expected);


  char *mov_expected[] = {"mov", "r2", ",", "#", "23"};
  string_separate_runner("mov r2, #23", ' ', ",#[]", 5, mov_expected);
}
