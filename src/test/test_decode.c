#include "unity.h"
#include "../decode.h"

instruction_t dp_tst_op2rotatedI = {
  .type = DP,
  .cond = 0x0,
  .i.dp = (dp_instruction_t){
    .padding = 0x0,
    .I = 1,
    .opcode = TST,
    .S = 1,
    .rn = 0x0,
    .rd = 0x0,
    .operand2 = { .imm = {
      .rotate = 0x2,
      .value = 0x8
    }
  }
}
instruction_t dp_ORR_op2shiftedR = {
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
      .shift = { .constant {
        .integer = 0x4;
          }
        },
      .type = ROR,
      .shiftBy = 0
      }
    }
  }
}
}
instruction_t dp_and_invalidreg_i = {
  .type = DP,
  .cond = 0xc,
  .i = {
    .dp = {
      .padding = 0x0,
      .I = 0,
      .opcode = And,
      .S = 1,
      .rn = 19,
      .rd = 27,
      .operand2 = { .imm = {
        .rotate = 0x0,
        .value = 0x1
      }
      }
    }
  }
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
  /* code */
}
