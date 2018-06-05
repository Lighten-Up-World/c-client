#include "../assemble/parser.h"
#include "test_decode.c"

void test_parse_hal(void){
  instruction_t hal_i = {
          .type = HAL,
          .cond = 0x0,
          .i.hal.pad0 = 0x0
  };
  token_t tokens[4] = {{.type=T_OPCODE, .str="andeq"},
                       {.type=T_REGISTER, .str="r0"},
                       {.type=T_REGISTER, .str="r0"},
                       {.type=T_REGISTER, .str="r0"} };
  instruction_t result;
  if (parse(tokens, &result, 4)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(hal_i, result);
}

void test_parse_mul(void){
  instruction_t mul_i = {
          .type = MUL,
          .cond = 0xE,
          .i.mul = {
                  .pad0 = 0x0,
                  .A = 0,
                  .S = 0,
                  .rd = 0xC,
                  .rn = 0xA,
                  .rs = 0x1,
                  .pad9 = 0x9,
                  .rm = 0x2
          }
  };
  token_t tokens[4] = {{.type=T_OPCODE, .str="mul"},
                       {.type=T_REGISTER, .str="r12"},
                       {.type=T_REGISTER, .str="r2"},
                       {.type=T_REGISTER, .str="r1"} };

  instruction_t result;
  if (parse(tokens, &result, 4)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(mul_i, result);
}

void test_parse_mla(void){
  instruction_t mla_i = {
          .type = MUL,
          .cond = 0xE,
          .i.mul = {
                  .pad0 = 0x0,
                  .A = 1,
                  .S = 0,
                  .rd = 0xC,
                  .rn = 0xA,
                  .rs = 0x1,
                  .pad9 = 0x9,
                  .rm = 0x2
          }
  };
  token_t tokens[5] = {{.type=T_OPCODE, .str="mla"},
                       {.type=T_REGISTER, .str="r12"},
                       {.type=T_REGISTER, .str="r2"},
                       {.type=T_REGISTER, .str="r1"},
                       {.type=T_REGISTER, .str="r10"}};

  instruction_t result;
  if (parse(tokens, &result, 5)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(mla_i, result);
}

void test_parse_dp(void){

  instruction_t mov_i = {
          .type = DP,
          .cond = 0xE,
          .i.dp = {
                  .opcode = MOV,
                  .rd = 1,
                  .operand2 = {
                          .imm.rotated.value = 56,
                          .imm.rotated.rotate = 0
                  };
          }
  };
  token_t tokens[3] = {{.type=T_OPCODE, .str="mov"},
                       {.type=T_REGISTER, .str="r1"},
                       {.type=T_REGISTER, .str="#56"}};

  instruction_t result;
  if (parse(tokens, &result, 3)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(mov_i, result);
}


