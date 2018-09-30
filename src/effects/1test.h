#ifndef TEST0_H
#define TEST0_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../utils/csv.h"
#include "../utils/list.h"
#include "../opc/opc_client.h"
#include "../extension.h"
#include "../pixel.h"

int test1_run(effect_runner_t* self);
effect_t *get_test1_effect(void * obj);

#endif
