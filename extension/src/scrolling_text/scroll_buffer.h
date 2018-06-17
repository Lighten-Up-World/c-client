#ifndef ARM11_22_SCROLL_BUFFER_H
#define ARM11_22_SCROLL_BUFFER_H

#include "../pixel.h"
#include "../extension.h"
#define PIXEL_FILE "../layout/CoordsToListPos.txt"

typedef struct {
  opc_pixel_t **grid;
  int pos;
  int width;
} buffer_t;

buffer_t *buffer_new(int cols);
void buffer_free(buffer_t *buff);
void clear_buffer(buffer_t *b);
void run(buffer_t *buff, double rate);

#endif
