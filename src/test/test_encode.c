#include "unity/unity.h"
#include "../assemble/encode.h"

void compareBinary(word_t expected, word_t result){
  TEST_ASSERT_EQUAL_MESSAGE(expected, result, "Binary does not match");
}

void test_encode_hal(void){
  word_t hal_w = 0x00000000;
  instruction_t hal_i = {
          .type = HAL,
          .cond = 0x0,
          .i.hal.pad0 = 0x0
  };
  word_t result;
  if(encode(&hal_i,&result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareBinary(hal_w, result);
}

void test_encode_mul(void){
  word_t mul_w = 0xD03CA192;
  instruction_t mul_i = {
          .type = MUL,
          .cond = 0xD,
          .i.mul = {
                  .pad0 = 0x0,
                  .A = 1,
                  .S = 1,
                  .rd = 0xC,
                  .rn = 0xA,
                  .rs = 0x1,
                  .pad9 = 0x9,
                  .rm = 0x2
          }
  };
  word_t result;
  if(encode(&mul_i,&result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareBinary(mul_w, result);
}
void test_encode_dp_rotated_immediate(void) {
  word_t dp_tst_op2rotatedI_w = 0x03100208;//0b0000 00 1 1000 1 0000 0000 0010 00001000;
  instruction_t dp_tst_op2rotatedI_i = {
          .type = DP,
          .cond = 0x0,
          .i.dp = {
                  .padding = 0x0,
                  .I = 1,
                  .opcode = TST,
                  .S = 1,
                  .rn = 0x0,
                  .rd = 0x0,
                  .operand2.imm.rotated = {
                          .rotate = 0x2,
                          .value = 0x8
                  }
          }
  };
  word_t result;
  if(encode(&dp_tst_op2rotatedI_i,&result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareBinary(dp_tst_op2rotatedI_w, result);
}

void test_encode_dp_shifted_register(void){
  word_t dp_ORR_op2shiftedR_w = 0xA1900260;
  instruction_t dp_ORR_op2shiftedR_i = {
          .type = DP,
          .cond = 0xA,
          .i.dp = {
                  .padding = 0x0,
                  .I = 0,
                  .opcode = ORR,
                  .S = 1,
                  .rn = 0x0,
                  .rd = 0x0,
                  .operand2.reg = {
                          .shift.constant.integer = 0x4,
                          .type = ROR,
                          .shiftBy = 0,
                          .rm = 0x0
                  },
          }
  };
  word_t result;
  if(encode(&dp_ORR_op2shiftedR_i,&result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareBinary(dp_ORR_op2shiftedR_w, result);
}


void test_encode_sdt(void){
  word_t sdt = 0xC5935000;
  instruction_t sdt_i = {
          .type = SDT,
          .cond = 0xC,
          .i.sdt = {
                  .pad1 = 0x1,
                  .I = 0,
                  .P = 1,
                  .U = 1,
                  .pad0 = 0x0,
                  .L = 1,
                  .rn = 0x3,
                  .rd = 0x5,
                  .offset.imm.fixed = 0x0
          }
  };
  word_t result;
  if(encode(&sdt_i,&result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareBinary(sdt, result);
}

void test_encode_sdt_with_invalidreg(void){
  word_t sdt_and_invalidreg_w = 0xC59EF000;
  instruction_t sdt_and_invalidreg_i = {
          .type = SDT,
          .cond = 0xC,
          .i.sdt = {
                  .pad1 = 0x1,
                  .I = 0,
                  .P = 1,
                  .U = 1,
                  .pad0 = 0x0,
                  .L = 1,
                  .rn = 0xE,
                  .rd = 0xF,
                  .offset.imm.fixed = 0x0
          }
  };
  word_t result;
  if(!encode(&sdt_and_invalidreg_i,&result)){
    TEST_ASSERT_MESSAGE(false, "Missing Error");
  }
}

void test_encodeBrn(void) {
    word_t brn_w = 0xAA000032;
    instruction_t brn_i = {
            .type = BRN,
            .cond = 0xA,
            .i.brn = {
                    .padA = 0xA,
                    .offset = 0x32
            }
    };
    word_t result;
    if(encode(&brn_i,&result)){
      TEST_ASSERT_MESSAGE(false, "False Error");
    }
    compareBinary(brn_w, result);
}
