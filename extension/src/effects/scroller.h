#ifndef SCROLLER_H
#define SCROLLER_H

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

typedef struct {
  opc_pixel_t **grid;
  int pos;
  int width;
} buffer_t;

typedef struct {
  buffer_t *buff;
  opc_pixel_t **pixel_grid;
} scroller_storage_t;

buffer_t *buffer_new(int cols);
void buffer_free(buffer_t *buff);
void clear_buffer(buffer_t *b);

int scroller_run(effect_runner_t* self);
effect_t *get_scroller_effect(void * obj);
void scroller_effect_delete(effect_t *self);

#endif
