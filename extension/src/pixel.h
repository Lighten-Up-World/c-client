#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

#include "api.h"

typedef uint32_t intensity_t;

typedef struct colour{
  uint32_t red;
  uint32_t green;
  uint32_t blue;
} colour_t;

typedef struct pixel{
  grid_t      grid;
  colour_t    colour;
  intensity_t intensity;
} pixel_t;

typedef struct map{
  pixel_t *pixels;
  int size;
} map_t;

// Node Functions
pixel_t *pixel_new(grid_t grid, colour_t colour, intensity_t intensity);

// List Construction
map_t *map_new();
void map_delete(map_t *self);

#endif
