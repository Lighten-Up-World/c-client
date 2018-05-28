#include "unity.h"
#include "../decode.h"
#include "../bitops.c"

void compareInstructions(instruction_t e, instruction_t d){
  TEST_ASSERT_EQUAL(e.type, d.type);
  TEST_ASSERT_EQUAL(e.cond, d.cond);
  if(e.type == DP){
    dp_instruction_t edp = e.i.dp;
    dp_instruction_t ddp = d.i.dp;
    TEST_ASSERT_EQUAL(edp.padding, ddp.padding);
    TEST_ASSERT_EQUAL(edp.I, ddp.I);
    TEST_ASSERT_EQUAL(edp.opcode, ddp.opcode);
    TEST_ASSERT_EQUAL(edp.S, ddp.S);
    TEST_ASSERT_EQUAL(edp.rn, ddp.rn);
    TEST_ASSERT_EQUAL(edp.rd, ddp.rd);
    if(edp.I){
      op_immediate_t eimm = edp.operand2.imm;
      op_immediate_t dimm = ddp.operand2.imm;
      TEST_ASSERT_EQUAL(eimm.rotated.rotate, dimm.rotated.rotate);
      TEST_ASSERT_EQUAL(eimm.rotated.value, dimm.rotated.value);
    }
    else {
      op_shiftreg_t ereg = edp.operand2.reg;
      op_shiftreg_t dreg = ddp.operand2.reg;
      TEST_ASSERT_EQUAL(ereg.type, dreg.type);
      TEST_ASSERT_EQUAL(ereg.shiftBy, dreg.shiftBy);
      TEST_ASSERT_EQUAL(ereg.rm, dreg.rm);
      if(ereg.shiftBy){
        TEST_ASSERT_EQUAL(ereg.shift.shiftreg.rs,
          dreg.shift.shiftreg.rs);
        TEST_ASSERT_EQUAL(ereg.shift.shiftreg.zeroPad,
          dreg.shift.shiftreg.zeroPad);
      }
      else {
        TEST_ASSERT_EQUAL(ereg.shift.constant.integer,
          dreg.shift.constant.integer);
      }
    }

  }
  if(e.type == SDT){
    sdt_instruction_t esdt = e.i.sdt;
    sdt_instruction_t dsdt = d.i.sdt;
    TEST_ASSERT_EQUAL(esdt.pad1, dsdt.pad1);
    TEST_ASSERT_EQUAL(esdt.I, dsdt.I);
    TEST_ASSERT_EQUAL(esdt.P, dsdt.P);
    TEST_ASSERT_EQUAL(esdt.U, dsdt.U);
    TEST_ASSERT_EQUAL(esdt.pad0, dsdt.pad0);
    TEST_ASSERT_EQUAL(esdt.L, dsdt.L);
    TEST_ASSERT_EQUAL(esdt.rn, dsdt.rn);
    TEST_ASSERT_EQUAL(esdt.rd, dsdt.rd);
    if(esdt.I){
      op_shiftreg_t ereg = esdt.offset.reg;
      op_shiftreg_t dreg = dsdt.offset.reg;
      TEST_ASSERT_EQUAL(ereg.type, dreg.type);
      TEST_ASSERT_EQUAL(ereg.shiftBy, dreg.shiftBy);
      TEST_ASSERT_EQUAL(ereg.rm, dreg.rm);
      if(ereg.shiftBy){
        TEST_ASSERT_EQUAL(ereg.shift.shiftreg.rs,
          dreg.shift.shiftreg.rs);
        TEST_ASSERT_EQUAL(ereg.shift.shiftreg.zeroPad,
          dreg.shift.shiftreg.zeroPad);
      }
      else {
        TEST_ASSERT_EQUAL(ereg.shift.constant.integer,
          dreg.shift.constant.integer);
      }
    }
    else {
      op_immediate_t eimm = esdt.offset.imm;
      op_immediate_t dimm = dsdt.offset.imm;
      TEST_ASSERT_EQUAL(eimm.fixed, dimm.fixed);
    }
  }
  if(e.type == MUL){
    mul_instruction_t emul = e.i.mul;
    mul_instruction_t dmul = d.i.mul;
    TEST_ASSERT_EQUAL(emul.pad0, dmul.pad0);
    TEST_ASSERT_EQUAL(emul.A, dmul.A);
    TEST_ASSERT_EQUAL(emul.S, dmul.S);
    TEST_ASSERT_EQUAL(emul.rd, dmul.rd);
    TEST_ASSERT_EQUAL(emul.rn, dmul.rn);
    TEST_ASSERT_EQUAL(emul.rs, dmul.rs);
    TEST_ASSERT_EQUAL(emul.pad9, dmul.pad9);
    TEST_ASSERT_EQUAL(emul.rm, dmul.rm);
  }
  if(e.type == BRN){
    brn_instruction_t ebrn = e.i.brn;
    brn_instruction_t dbrn = d.i.brn;
    TEST_ASSERT_EQUAL(ebrn.pad5, dbrn.pad5);
    TEST_ASSERT_EQUAL(ebrn.pad0, dbrn.pad0);
    TEST_ASSERT_EQUAL(ebrn.offset, dbrn.offset);
  }
  if(e.type == HAL){
    TEST_ASSERT_EQUAL(e.i.hal.pad0, d.i.hal.pad0);
  }
}

void test_decodeDp_rotated_immediate(void) {
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
  instruction_t decoded = decodeWord(dp_tst_op2rotatedI_w);
  instruction_t expected = dp_tst_op2rotatedI_i;
  compareInstructions(expected, decoded);
}
void test_decodeDp_shifted_register(void) {
  word_t dp_ORR_op2shiftedR_w = 0x0190026;
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
  instruction_t decoded = decodeWord(dp_ORR_op2shiftedR_w);
  instruction_t expected = dp_ORR_op2shiftedR_i;
  compareInstructions(expected, decoded);
}
void test_decodeSDT_invalid_register(void) {
  word_t sdt_and_invalidreg_w = 0xC59EF000;
  instruction_t sdt_and_invalidreg_i = {
    .type = SDT,
    .cond = 0xc,
    .i = {
      .sdt = {
        .pad1 = 0x1,
        .I = 0,
        .P = 1,
        .U = 1,
        .pad0 = 0x0, //Unsupported registers
        .L = 1,
        .rn = 0xE,
        .rd = 0xF,
        .offset = { .imm = {
          .fixed = 0x0
        }
        }
      }
    }
  };
  instruction_t decoded = decodeWord(sdt_and_invalidreg_w);
  instruction_t expected = sdt_and_invalidreg_i;
  compareInstructions(expected, decoded);

}
void test_decodeMul(void) {
  word_t mul_w = 0xD03CA192;
  instruction_t mul_i = {
    .type = MUL,
    .cond = 0xD,
    .i = {
      .mul = {
        .pad0 = 0x0,
        .A = 1,
        .S = 1,
        .rd = 0xC,
        .rn = 0xA,
        .rs = 0x1,
        .pad9 = 0x9,
        .rm = 0x2
        }
      }
    };
  instruction_t decoded = decodeWord(mul_w);
  instruction_t expected = mul_i;
  compareInstructions(expected, decoded);
}
void test_decodeBrn(void) {
  word_t brn_w = 0xAA000032;
  instruction_t brn_i = {
    .type = BRN,
    .cond = 0xA,
    .i = {
      .brn = {
        .pad5 = 0x5,
        .pad0 = 0x0,
        .offset = 0x32
        }
      }
    };
  instruction_t decoded = decodeWord(brn_w);
  instruction_t expected = brn_i;
  compareInstructions(expected, decoded);
}
void test_decodeHal(void) {
  word_t hal_w = 0x00000000;
  instruction_t hal_i = {
    .type = HAL,
    .cond = 0x0,
    .i = {
      .hal = {
        .pad0 = 0x0
        }
      }
  };
  instruction_t decoded = decodeWord(hal_w);
  instruction_t expected = hal_i;
  compareInstructions(expected, decoded);
}
