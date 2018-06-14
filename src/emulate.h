#ifndef EMULATE_H
#define EMULATE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "utils/arm.h"
#include "utils/instructions.h"
#include "utils/io.h"
#include "utils/register.h"

#include "emulate/execute.h"
#include "emulate/decode.h"


int main(int argc, char **argv);
#endif
