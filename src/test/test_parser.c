#include "unity/unity.h"
#include "../assemble/symbolmap.h"
#include "../assemble/referencemap.h"
#include "../utils/bitops.h"
#include "../assemble/parser.h"

program_t *prog = NULL;

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
  token_list_t tkns = {tokens, 4};
  instruction_t result;
  if (parse(prog, &tkns, &result)){
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
  token_list_t tkns = {tokens, 4};
  instruction_t result;
  if (parse(prog, &tkns, &result)){
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
 token_list_t tkns = {tokens, 5};
  instruction_t result;
  if (parse(prog, &tkns, &result)){
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
                  }
          }
  };
  token_t tokens[3] = {{.type=T_OPCODE, .str="mov"},
                       {.type=T_REGISTER, .str="r1"},
                       {.type=T_HASH_EXPR, .str="#56"}};
  token_list_t tkns = {tokens, 3};
  instruction_t result;
  if (parse(prog, &tkns, &result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(mov_i, result);
}

void test_parse_sdt_imm(void) {

  instruction_t sdt_i = {
      .type = SDT,
      .cond = 0xE,
      .i.sdt = {
          .pad1 = 0x1,
          .I = 0,
          .P = 1,
          .U = 1,
          .pad0 = 0x0,
          .L = 1,
          .rn = 15,
          .rd = 0,
          .offset.imm.fixed = 0x555
      }
  };
  token_t tokens[3] = {{.type=T_OPCODE, .str="ldr"},
                      {.type=T_REGISTER, .str="r0"},
                      {.type=T_EQ_EXPR, .str="=0x555"}};
  token_list_t tkns = {tokens, 3};
  instruction_t result;
  if (parse(prog, &tkns, &result)){
    TEST_ASSERT_MESSAGE(false, "False Error");
  }
  compareInstructions(sdt_i, result);
}
