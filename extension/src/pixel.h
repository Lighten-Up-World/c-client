#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>
#include "apimanager.h"

#define GRID_WIDTH 52
#define GRID_HEIGHT 24

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

typedef struct pixel{
  grid_t      grid;
  colour_t    colour;
  intensity_t intensity;
} pixel_t;

// Node Functions
pixel_t *pixel_new(grid_t grid, colour_t colour, intensity_t intensity);

#endif
