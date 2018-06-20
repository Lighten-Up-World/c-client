#ifndef EXTENSION_H
#define EXTENSION_H

#include <stdint.h>
#include <time.h>
#include "utils/list.h"

#define MILLI_TO_NANO 100000
#define HOST_AND_PORT "127.0.0.1:7890"

#define GRID_WIDTH 53
#define GRID_HEIGHT 24
#define NUM_PIXELS 471

#define PIXEL_COLOUR_MAX 255
#define PIXEL_COLOUR_MIN 0
#define WHITE_PIXEL (opc_pixel_t) {PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX}
#define BLACK_PIXEL (opc_pixel_t) {PIXEL_COLOUR_MIN, PIXEL_COLOUR_MIN, PIXEL_COLOUR_MIN}

/* Defines an rgb pixel for use in opc simulations */
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} opc_pixel_t;

typedef struct {
  opc_pixel_t pixels[NUM_PIXELS];
} frame_t;

typedef struct grid {
  int x;
  int y;
} grid_t;

typedef struct effect effect_t;
typedef struct effect_runner effect_runner_t;

typedef int (*get_frame_func) (effect_runner_t *self, frame_t *frame);
typedef int (*get_pixel_func) (effect_runner_t *self, int pos);
typedef int (*effect_runner_func)(effect_runner_t *self);
typedef void (*effect_func)(effect_t *self);

struct effect {
  get_frame_func get_frame;
  get_pixel_func get_pixel;
  struct timespec time_delta;
  effect_runner_func run;
  effect_func remove;
  void *obj;
};

struct effect_runner{
  effect_t *effect;
  list_t *pixel_info;
  frame_t *frame;
  int8_t sink;
  long frame_no;
};

typedef uint32_t intensity_t;

typedef struct geolocation {
  double latitude;
  double longitude;
} geolocation_t;

typedef struct {
  int channel;
  int num;
} strip_t;

typedef struct {
  grid_t grid;
  geolocation_t geo;
  strip_t strip;
} pixel_info_t;

#endif //ARM11_22_EXTENSION_H
