#include "unity.h"
#include "../decode.h"
#include "../bitops.c"

word_t dp_tst_op2rotatedI_w = 0x03100208;//0b0000 00 1 1000 1 0000 0000 0010 00001000;
instruction_t dp_tst_op2rotatedI_i = {
  .type = DP,
  .cond = 0x0,
  .i.dp = (dp_instruction_t){
    .padding = 0x0,
    .I = 1,
    .opcode = TST,
    .S = 1,
    .rn = 0x0,
    .rd = 0x0,
    .operand2 = { .imm = { .rotated = {
      .rotate = 0x2,
      .value = 0x8
    }
    }
  }
}
};
instruction_t dp_ORR_op2shiftedR_i = {
  .type = DP,
  .cond = 0xf,
  .i = {
    .dp = {
    .padding = 0x0,
    .I = 0,
    .opcode = ORR,
    .S = 1,
    .rn = 0x0,
    .rd = 0x0,
    .operand2 = { .reg = {
      .shift = { .constant = {
        .integer = 0x4
          }
        },
      .type = ROR,
      .shiftBy = 0
      }
    }
  }
}
};
instruction_t dp_and_invalidreg_i = {
  .type = DP,
  .cond = 0xc,
  .i = {
    .dp = {
      .padding = 0x0,
      .I = 0,
      .opcode = AND,
      .S = 1,
      .rn = 14, //Unsupported registers
      .rd = 15,
      .operand2 = { .imm = {
        .fixed = 0x0
      }
      }
    }
  }
};

void compareInstructions(instruction_t e, instruction_t d){
  TEST_ASSERT_EQUAL(e.type, d.type);
  TEST_ASSERT_EQUAL(e.cond, d.cond);
  TEST_ASSERT_EQUAL(e.i.dp.padding, d.i.dp.padding);
  TEST_ASSERT_EQUAL(e.i.dp.I, d.i.dp.I);
  TEST_ASSERT_EQUAL(e.i.dp.opcode, d.i.dp.opcode);
  TEST_ASSERT_EQUAL(e.i.dp.S, d.i.dp.S);
  TEST_ASSERT_EQUAL(e.i.dp.rn, d.i.dp.rn);
  TEST_ASSERT_EQUAL(e.i.dp.rd, d.i.dp.rd);
  TEST_ASSERT_EQUAL(e.i.dp.operand2.imm.rotated.rotate,
    d.i.dp.operand2.imm.rotated.rotate);
    TEST_ASSERT_EQUAL(e.i.dp.operand2.imm.rotated.value,
      d.i.dp.operand2.imm.rotated.value);
}

void test_decodeOp(void) {
  /* code */
}

void test_decodeDp(void) {
  /* code */
}
void test_decodeMul(void) {
  /* code */
}
void test_decodeBrn(void) {
  /* code */
}
void test_decodeType(void) {
  /* code */
}

void test_decodeWord(void) {
  instruction_t decoded = decodeWord(dp_tst_op2rotatedI_w);
  instruction_t expected = dp_tst_op2rotatedI_i;
  compareInstructions(expected, decoded);
  printf("D: %08x E: %08x\n", expected.i.dp, decoded.i.dp);
}
