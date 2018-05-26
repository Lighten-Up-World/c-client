#ifndef EXEVUTE_H
#define EXECUTE_H

#include "arm.h"

int condition(state_t *state);

void execute(state_t *state);
void executeDPI(state_t *state);
void executeMUL(state_t *state);
void executeBRN(state_t *state);
void executeSDT(state_t *state);
void executeHAL(state_t *state);

#endif
