#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>
#include <stdlib.h>
#include "utils/list.h"

#define GRID_WIDTH 53
#define GRID_HEIGHT 24
#define NUM_PIXELS 471

#define PIXEL_COLOUR_MAX 255.0

typedef uint32_t intensity_t;

typedef struct colour{
  uint32_t red;
  uint32_t green;
  uint32_t blue;
} colour_t;

typedef struct grid{
    int x;
    int y;
} grid_t;

/* Defines an rgb pixel for use in opc simulations */
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_t;

typedef struct geolocation{
  double latitude;
  double longitude;
} geolocation_t;

typedef struct {
  grid_t grid;
  geolocation_t geo;
} pixel_info_t;

pixel_t **grid_new(int cols, int rows);
pixel_t **pixel_grid_new(void);
void grid_free(pixel_t **pixel_grid);
int get_pos(int x, int y, list_t *pixel_info);

#endif
