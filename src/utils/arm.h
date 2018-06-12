/*
 *  Breakdown ARM format into internal representation for state and registers
 *  using data structures and constants.
 */

#ifndef ARM_H
#define ARM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef DEBUG
  #define DEBUG_PRINT(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
  #define DEBUG_CMD(cmd) \
        do { cmd; } while (0)
#else
  #define DEBUG_PRINT(fmt, ...)
  #define DEBUG_CMD(cmd)
#endif

#define NUM_GENERAL_REGISTERS 13
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

#include "instructions.h"

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
