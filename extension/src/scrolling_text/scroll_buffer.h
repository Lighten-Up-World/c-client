#ifndef ARM11_22_SCROLL_BUFFER_H
#define ARM11_22_SCROLL_BUFFER_H

#include <stdlib.h>
#include "../opc/opc.h"
#include "../pixel.h"

#define WHITE_PIXEL (pixel_t) {255, 255, 255}
#define PIXEL_FILE "../layout/CoordsToListPos.txt"
#define MILLI_TO_NANO 100000
#define HOST_AND_PORT "127.0.0.1:7890"

typedef struct {
  pixel_t **grid;
  int pos;
  int width;
} buffer_t;

buffer_t *buffer_new(int cols);
void buffer_free(buffer_t *buff);
void clear_buffer(buffer_t *b);
void run(buffer_t *buff, double rate);

#endif
