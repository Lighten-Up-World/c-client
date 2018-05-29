/*
 *  Breakdown ARM format into internal representation for state and registers
 *  using data structures and constants.
 */

#ifndef ARM_H
#define ARM_H

#include <stdint.h>
#include <stdbool.h>

#define DEBUG 0

#define DEBUG_PRINT_2(x, y) do {if(DEBUG){printf(x, y);}} while(0)
#define DEBUG_PRINT_3(x, y, z) do {if(DEBUG){printf(x, y, z);}} while(0)

// Total number of registers
#define NUM_GENERAL_REGISTERS 13

// Memory Size
#define MEM_SIZE 65536

typedef uint8_t byte_t;
typedef uint8_t reg_address_t;
typedef uint16_t address_t;
typedef uint32_t word_t;
typedef bool flag_t;

typedef struct {
  word_t value;
  flag_t carry;
} shift_result_t;

typedef enum {
  EQ = 0x0, // Equal
  NE = 0x1, // Not Equal
  GE = 0xA, // Greater or equal
  LT = 0xB, // Less than
  GT = 0xC, // Greater than
  LE = 0xD, // Less than or equal
  AL = 0xE  // No condition
} condition_t;

typedef enum {
  N = 0x8, // Negative
  Z = 0x4, // Zero
  C = 0x2, // Carried out
  V = 0x1  // Overflowed
} cpsr_flag_t;

typedef enum {
  AND = 0x0, // Rn AND op2
  EOR = 0x1, // Rn EOR op2
  SUB = 0x2, // Rn - op2
  RSB = 0x3, // op2 - Rn
  ADD = 0x4, // Rn + op2
  TST = 0x8, // and, no result
  TEQ = 0x9, // eor, no result
  CMP = 0xA, // sub, no result
  ORR = 0xC, // Rn OR op2
  MOV = 0xD  // op2
} opcode_t;

typedef enum {
  // DATA PROCESSING
  M_ADD, // Rd := Rn + Op2
  M_SUB, // Rd := Rn - Op2
  M_RSB, // Rd := Op2 - Rn
  M_AND, // Rd := Rn && Op2
  M_EOR, // Rd := Rn XOR Op2
  M_ORR, // Rd := Rn || Op2
  M_MOV, // Rd := Op2
  M_TST, // CPSR := Rn && Op2
  M_TEQ, // CPSR := Rn XOR Op2
  M_CMP, // CPSR := Rn - Op2

  // MULTIPLY
  M_MUL, // Rd := Rm x Rs
  M_MLA, // Rd := (Rm x Rs) = Rn

  // SINGLE DATA TRANSFER
  M_LDR, // Rd := (address)
  M_STR, // (address) := Rd

  // BRANCH
  M_BEQ, // PC := address
  M_BNE, // PC := address
  M_BLT, // PC := address
  M_BGT, // PC := address
  M_BLE, // PC := address
  M_B,   // PC := address

  // SPECIAL
  M_LSL,  // Rd := Rd <<
  M_ANDEQ // Halt
} mnemonic_t;

#include "instructions.h"  // is there a reason this is so far down?

typedef struct {
  word_t fetched;
  instruction_t *decoded;
} pipeline_t;

typedef struct {
  word_t r[NUM_GENERAL_REGISTERS];
  word_t pc;
  word_t cpsr;
} registers_t;

typedef struct {
  registers_t registers;
  byte_t memory[MEM_SIZE];
  pipeline_t pipeline;
} state_t;

#endif
