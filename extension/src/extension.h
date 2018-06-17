#ifndef ARM11_22_EXTENSION_H
#define ARM11_22_EXTENSION_H

#include <stdint.h>

#define MILLI_TO_NANO 100000
#define HOST_AND_PORT "127.0.0.1:7890"

#define GRID_WIDTH 53
#define GRID_HEIGHT 24
#define NUM_PIXELS 471

#define PIXEL_COLOUR_MAX 255
#define WHITE_PIXEL (opc_pixel_t) {PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX}

/* Defines an rgb pixel for use in opc simulations */
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} opc_pixel_t;

typedef struct grid {
  int x;
  int y;
} grid_t;

typedef uint32_t intensity_t;

typedef struct geolocation {
  double latitude;
  double longitude;
} geolocation_t;

typedef struct {
  grid_t grid;
  geolocation_t geo;
} pixel_info_t;

#endif //ARM11_22_EXTENSION_H
