#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

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

/* Defines an rgb pixel for use in opc simulations */
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_t;

// typedef struct pixel{
//   grid_t      grid;
//   colour_t    colour;
// } pixel_t;

typedef struct geolocation{
  double latitude;
  double longitude;
} geolocation_t;

typedef struct {
  grid_t grid;
  geolocation_t geo;
} pixel_info_t;

// Node Functions
pixel_t *pixel_new(grid_t grid, colour_t colour, intensity_t intensity);

#endif
