/*
* Breaksdown ARM format into internal representation for state and registers
* into datastructures and constants
*/

#ifndef ARM_H
#define ARM_H
#include <stdint.h>
#include <stdbool.h>

// Total number of registers
#define NUM_GENERAL_REGISTERS 13
// Memory Size
#define MEM_SIZE 65536
// Instruction word size
#define INSTR_SIZE 32

typedef uint8_t byte_t;
typedef int8_t reg_address_t;
typedef uint16_t address_t;
typedef uint32_t word_t;
typedef uint32_t val_t;

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
  DP,  // Data processing
  MUL, // Multiply instruction
  SDT, // Single data transfer
  BRN, // Branch instruction
  HAL  // Halt instruction
} instruction_type_t;

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
  //// DATA PROCESSING ////
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
  //// MULTIPLY ////
  M_MUL, // Rd := Rm x Rs
  M_MLA, // Rd := (Rm x Rs) = Rn
  //// SINGLE DATA TRANSFER ////
  M_LDR, // Rd := (address)
  M_STR, // (address) := Rd
  //// BRANCH ////
  M_BEQ, // PC := address
  M_BNE, // PC := address
  M_BLT, // PC := address
  M_BGT, // PC := address
  M_BLE, // PC := address
  M_B,   // PC := address
  //// SPECIAL ////
  M_LSL,  // Rd := Rd <<
  M_ANDEQ // Halt
} mnemonic_t;

typedef enum {
  LSL = 0x0, // Logical left
  LSR = 0x1, // Logical right
  ASR = 0x2, // Arithmetic right
  ROR = 0x3  // Rotate cyclically
} shift_type_t;

typedef struct {
  // Immediate Operand
  bool I; //DPI: 1 -> Operand2 is an immediate constant
          //     0 -> Operand2 is a shifted register
          //SDT: 1 -> Offset is a shifted register
          //     0 -> = interpreted as an unsigned 12 bit immediate offset
  // Accumulate
  bool A; // Multiply and accumulate, else just multiply
  // Set conditions codes
  bool S; // CPSR flags updated during execution
  // Pre/Post Indexing bit
  bool P; // 1 -> offset added/subtracted to the base register before transfer
          // 0 -> offset is added/subtracted to the br after transferring.
  // Up bit
  bool U; // 1 -> Offset added to the br.
          // 0 -> Offset is subtracted from the br
  //Load/Store
  bool L; // 1 -> Word loaded from memory
          // 0 -> Word is stored into memory
} flags_t;

//TODO: Add bitfield for compacting.
typedef struct {
  instruction_type_t type;
  byte_t cond;
  opcode_t operation;

  val_t immediate_value;

  reg_address_t rn;
  reg_address_t rd;
  reg_address_t rs;
  reg_address_t rm;

  flags_t flags;

  shift_type_t shift_type;
  byte_t shift_amount;

} instruction_t;

typedef struct {
  word_t fetched;
  instruction_t *decoded_instruction;
} pipeline_t;

typedef struct {
  word_t r[NUM_GENERAL_REGISTERS];
  word_t sp;
  word_t lr;
  word_t pc;
  word_t cpsr;
} registers_t;

typedef struct {
  registers_t registers;
  byte_t memory[MEM_SIZE];
  pipeline_t pipeline;
} state_t;

#endif
