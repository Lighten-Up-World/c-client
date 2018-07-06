#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../../../src/utils/error.h"
#include "../utils/csv.h"
#include "../utils/list.h"
#include "../opc/opc_client.h"
#include "../extension.h"
#include "../pixel.h"

int test_run(effect_runner_t* self);
effect_t *get_test_effect(void * obj);

#endif
