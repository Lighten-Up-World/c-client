#include "../../unity/src/unity.h"
#include "../emulate/decode.h"
#include "../utils/bitops.c"

void compareInstructions(instruction_t e, instruction_t d){
  TEST_ASSERT_EQUAL_MESSAGE(e.type, d.type, "Instr Type");
  TEST_ASSERT_EQUAL_MESSAGE(e.cond, d.cond, "Condition");
  if(e.type == DP){
    dp_instruction_t edp = e.i.dp;
    dp_instruction_t ddp = d.i.dp;
    TEST_ASSERT_EQUAL_MESSAGE(edp.padding, ddp.padding, "Padding");
    TEST_ASSERT_EQUAL_MESSAGE(edp.I, ddp.I, "I flag");
    TEST_ASSERT_EQUAL_MESSAGE(edp.opcode, ddp.opcode, "Opcode");
    TEST_ASSERT_EQUAL_MESSAGE(edp.S, ddp.S, "S flag");
    TEST_ASSERT_EQUAL_MESSAGE(edp.rn, ddp.rn, "Rn");
    TEST_ASSERT_EQUAL_MESSAGE(edp.rd, ddp.rd, "Rd");
    if(edp.I){
      op_immediate_t eimm = edp.operand2.imm;
      op_immediate_t dimm = ddp.operand2.imm;
      TEST_ASSERT_EQUAL_MESSAGE(eimm.rotated.rotate, dimm.rotated.rotate, "Rotated:R");
      TEST_ASSERT_EQUAL_MESSAGE(eimm.rotated.value, dimm.rotated.value, "Rotated:Value");
    }
    else {
      op_shiftreg_t ereg = edp.operand2.reg;
      op_shiftreg_t dreg = ddp.operand2.reg;
      TEST_ASSERT_EQUAL_MESSAGE(ereg.type, dreg.type, "Reg Type");
      TEST_ASSERT_EQUAL_MESSAGE(ereg.shiftBy, dreg.shiftBy, "ShiftBy");
      TEST_ASSERT_EQUAL_MESSAGE(ereg.rm, dreg.rm, "Rm");
      if(ereg.shiftBy){
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.shiftreg.rs,
                                  dreg.shift.shiftreg.rs, "Rs");
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.shiftreg.zeroPad,
                                  dreg.shift.shiftreg.zeroPad, "zeroPad");
      }
      else {
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.constant.integer,
                                  dreg.shift.constant.integer, "Integer");
      }
    }

  }
  if(e.type == SDT){
    sdt_instruction_t esdt = e.i.sdt;
    sdt_instruction_t dsdt = d.i.sdt;
    TEST_ASSERT_EQUAL_MESSAGE(esdt.pad1, dsdt.pad1, "pad1");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.I, dsdt.I, "I");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.P, dsdt.P, "P");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.U, dsdt.U, "U");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.pad0, dsdt.pad0, "pad0");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.L, dsdt.L, "L");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.rn, dsdt.rn, "Rn");
    TEST_ASSERT_EQUAL_MESSAGE(esdt.rd, dsdt.rd, "Rd");
    if(esdt.I){
      op_shiftreg_t ereg = esdt.offset.reg;
      op_shiftreg_t dreg = dsdt.offset.reg;
      TEST_ASSERT_EQUAL_MESSAGE(ereg.type, dreg.type, "Reg Type");
      TEST_ASSERT_EQUAL_MESSAGE(ereg.shiftBy, dreg.shiftBy, "ShiftBy");
      TEST_ASSERT_EQUAL_MESSAGE(ereg.rm, dreg.rm, "Rm");
      if(ereg.shiftBy){
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.shiftreg.rs,
                                  dreg.shift.shiftreg.rs, "Rs");
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.shiftreg.zeroPad,
                                  dreg.shift.shiftreg.zeroPad, "Zero Pad");
      }
      else {
        TEST_ASSERT_EQUAL_MESSAGE(ereg.shift.constant.integer,
                                  dreg.shift.constant.integer, "Integer");
      }
    }
    else {
      op_immediate_t eimm = esdt.offset.imm;
      op_immediate_t dimm = dsdt.offset.imm;
      TEST_ASSERT_EQUAL_MESSAGE(eimm.fixed, dimm.fixed, "Fixed");
    }
  }
  if(e.type == MUL){
    mul_instruction_t emul = e.i.mul;
    mul_instruction_t dmul = d.i.mul;
    TEST_ASSERT_EQUAL_MESSAGE(emul.pad0, dmul.pad0, "pad0");
    TEST_ASSERT_EQUAL_MESSAGE(emul.A, dmul.A, "A");
    TEST_ASSERT_EQUAL_MESSAGE(emul.S, dmul.S, "S");
    TEST_ASSERT_EQUAL_MESSAGE(emul.rd, dmul.rd, "Rd");
    TEST_ASSERT_EQUAL_MESSAGE(emul.rn, dmul.rn, "Rn");
    TEST_ASSERT_EQUAL_MESSAGE(emul.rs, dmul.rs, "Rs");
    TEST_ASSERT_EQUAL_MESSAGE(emul.pad9, dmul.pad9, "pad9");
    TEST_ASSERT_EQUAL_MESSAGE(emul.rm, dmul.rm, "Rm");
  }
  if(e.type == BRN){
    brn_instruction_t ebrn = e.i.brn;
    brn_instruction_t dbrn = d.i.brn;
    TEST_ASSERT_EQUAL_MESSAGE(ebrn.padA, dbrn.padA, "padA");
    TEST_ASSERT_EQUAL_MESSAGE(ebrn.offset, dbrn.offset, "offset");
  }
  if(e.type == HAL){
    TEST_ASSERT_EQUAL_MESSAGE(e.i.hal.pad0, d.i.hal.pad0, "pad0");
  }
}

void test_decodeDp_rotated_immediate(void) {
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
  instruction_t decoded;
  decode_word(&decoded, dp_tst_op2rotatedI_w);
  instruction_t expected = dp_tst_op2rotatedI_i;
  compareInstructions(expected, decoded);
}
void test_decodeDp_shifted_register(void) {
  word_t dp_ORR_op2shiftedR_w = 0xF1900260;
  instruction_t dp_ORR_op2shiftedR_i = {
      .type = DP,
      .cond = 0xF,
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
  instruction_t decoded;
  decode_word(&decoded, dp_ORR_op2shiftedR_w);
  instruction_t expected = dp_ORR_op2shiftedR_i;
  compareInstructions(expected, decoded);
}
void test_decodeSDT_invalid_register(void) {
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
  instruction_t decoded;
  decode_word(&decoded, sdt_and_invalidreg_w);
  instruction_t expected = sdt_and_invalidreg_i;
  compareInstructions(expected, decoded);

}
void test_decodeMul(void) {
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
  instruction_t decoded;
  decode_word(&decoded, mul_w);
  instruction_t expected = mul_i;
  compareInstructions(expected, decoded);
}

void test_decodeBrn(void) {
  word_t brn_w = 0xAA000032;
  instruction_t brn_i = {
      .type = BRN,
      .cond = 0xA,
      .i.brn = {
          .padA = 0xA,
          .offset = 0x32
      }
  };
  instruction_t decoded;
  decode_word(&decoded, brn_w);
  instruction_t expected = brn_i;
  compareInstructions(expected, decoded);
}

void test_decodeBrn2(void) {
  word_t brn_w = 0xAAFFFFFF;
  instruction_t brn_i = {
      .type = BRN,
      .cond = 0xA,
      .i.brn = {
          .padA = 0xA,
          .offset = 0xFFFFFF
      }
  };
  instruction_t decoded;
  decode_word(&decoded, brn_w);
  instruction_t expected = brn_i;
  compareInstructions(expected, decoded);
}

void test_decodeBrn3(void) {
  word_t brn_w = 0x3A800001;
  instruction_t brn_i = {
      .type = BRN,
      .cond = 0x3,
      .i.brn = {
          .padA = 0xA,
          .offset = 0x800001
      }
  };
  instruction_t decoded;
  decode_word(&decoded, brn_w);
  instruction_t expected = brn_i;
  compareInstructions(expected, decoded);
}

void test_decodeHal(void) {
  word_t hal_w = 0x00000000;
  instruction_t hal_i = {
      .type = HAL,
      .cond = 0x0,
      .i.hal.pad0 = 0x0
  };
  instruction_t decoded;
  decode_word(&decoded, hal_w);
  instruction_t expected = hal_i;
  compareInstructions(expected, decoded);
}
