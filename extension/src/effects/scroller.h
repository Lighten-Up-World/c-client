#ifndef SCROLL_BUFFER_H
#define SCROLL_BUFFER_H

#include "../pixel.h"
#include "../extension.h"

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
int run(effect_runner_t* self);
effect_t *get_scroller_effect(void);

#endif
