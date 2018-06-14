#ifndef EMULATE_H
#define EMULATE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "utils/arm.h"
#include "utils/instructions.h"

//Forward Definitions
typedef struct pipeline pipeline_t;
typedef struct registers registers_t;
typedef struct emulate_state emulate_state_t;

#include "utils/io.h"
#include "utils/register.h"

#include "emulate/execute.h"
#include "emulate/decode.h"

struct pipeline{
  word_t fetched;
  instruction_t *decoded;
};

struct registers{
  word_t r[NUM_GENERAL_REGISTERS];
  word_t pc;
  word_t cpsr;
};

struct emulate_state{
  registers_t registers;
  byte_t memory[MEM_SIZE];
  pipeline_t pipeline;
};
#endif
