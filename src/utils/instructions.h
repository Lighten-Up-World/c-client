/*
 * Contains Data-Types for representing decoded instructions
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "arm.h"

typedef enum {
  DP,  // Data processing
  MUL, // Multiply instruction
  SDT, // Single data transfer
  BRN, // Branch instruction
  HAL  // Halt instruction
} instruction_type_t;

typedef enum {
  LSL = 0x0, // Logical left
  LSR = 0x1, // Logical right
  ASR = 0x2, // Arithmetic right
  ROR = 0x3  // Rotate cyclically
} shift_type_t;

typedef struct {
  byte_t rotate : 4;
  byte_t value : 8;
} op_rotate_immediate_t;

typedef union {
  op_rotate_immediate_t rotated;
  word_t fixed: 12;
} op_immediate_t;

typedef struct {
  byte_t integer : 5;
} op_shift_const_t;

typedef struct {
  reg_address_t rs : 4;
  byte_t zeroPad : 1;
} op_shift_register_t;

typedef union {
  op_shift_const_t constant;
  op_shift_register_t shiftreg;
} op_shift_t;

typedef struct {
  op_shift_t shift;
  shift_type_t type : 2;
  flag_t shiftBy : 1;
  reg_address_t rm : 4;
} op_shiftreg_t;

typedef union {
  op_immediate_t imm;
  op_shiftreg_t reg;
} operand_t;

/*
 * DATA PROCESSING INSTRUCTION FORMAT
 * |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
 * |   COND    | 0| 0| I|   OPCODE  | S|     Rn    |
 * |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 * |     Rd    |             OPERAND               |
 */
typedef struct {
  byte_t padding : 2;
  // Immediate Operand
  flag_t I : 1;  // DPI: 1 -> Operand2 is an immediate constant
                 //      0 -> Operand2 is a shifted register
  opcode_t opcode : 4;
  // Set conditions codes
  flag_t S : 1; // CPSR flags updated during execution

  reg_address_t rn : 4;
  reg_address_t rd : 4;
  operand_t operand2;

} dp_instruction_t;


/*
 * MULTIPLY INSTRUCTION FORMAT
 * |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
 * |   COND    | 0| 0| 0| 0| 0| 0| A| S|     Rd    |
 * |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 * |     Rn    |     Rs    | 1| 0| 0| 1|     Rm    |
 */
typedef struct {
  byte_t pad0 : 6;          // 0x000000
  flag_t A : 1;             // Accumulate
  flag_t S : 1;             // Set conditions codes
  reg_address_t rd : 4;
  reg_address_t rn : 4;
  reg_address_t rs : 4;
  byte_t pad9 : 4;         // 0x1001
  reg_address_t rm : 4;
} mul_instruction_t;

/* SINGLE DATA TRANSFER INSTRUCTION FORMAT
 * |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
 * |   COND    | 0| 1| I| P| U| 0| 0| L|     Rn    |
 * |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 * |     Rd    |             OFFSET                |
 */
typedef struct {
  byte_t pad1 : 2; // 0x01
  //SDT: 1 -> Offset is a shifted register
  //     0 -> = interpreted as an unsigned 12 bit immediate offset
  flag_t I : 1;
  // Pre/Post Indexing bit
  // 1 -> offset added/subtracted to the base register before transfer
  // 0 -> offset is added/subtracted to the br after transferring.
  flag_t P : 1;
  // Up bit// Set conditions codes
  // 1 -> Offset added to the br.
  // 0 -> Offset is subtracted from the br
  flag_t U : 1;
  byte_t pad0 : 2; // 0x00
  //Load/Store 1 -> Word loaded from memory
  // 0 -> Word is stored into memory
  flag_t L : 1;
  reg_address_t rn : 4;
  reg_address_t rd : 4;
  operand_t offset;
} sdt_instruction_t;

/* BRANCH INSTRUCTION FORMAT
 * |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|
 * |   COND    | 1| 0| 1| 0|         OFFSET-
 * |15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 * |           -OFFSET                             |
 */
typedef struct {
  byte_t padA : 4;      // 0x1010
  word_t offset : 24;
} brn_instruction_t;

typedef struct {
  word_t pad0 : 32;     // 0x0000
} hal_instruction_t;

typedef union {
  dp_instruction_t dp;
  mul_instruction_t mul;
  sdt_instruction_t sdt;
  brn_instruction_t brn;
  hal_instruction_t hal;
} instructions_t;

typedef struct {
  instruction_type_t type;
  byte_t cond : 4;
  instructions_t i;
} instruction_t;

#endif
