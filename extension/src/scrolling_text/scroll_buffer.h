#ifndef ARM11_22_SCROLL_BUFFER_H
#define ARM11_22_SCROLL_BUFFER_H

#include "../opc/opc.h"

#define COLS 53
#define ROWS 24
#define PIXELS 471
#define WHITE_PIXEL (pixel) {255, 255, 255}
#define CONFIG_FILE "../layout/CoordsToListPos.txt"
#define MILLI_TO_NANO 100000
#define HOST_AND_PORT "127.0.0.1:7890"

typedef struct {
  pixel **grid;
  int width;
} buffer;

buffer *buffer_new(int cols);
void buffer_free(buffer *buff);
void clear_buffer(buffer *b);
void run(buffer *buff, double rate);

#endif
