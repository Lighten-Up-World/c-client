#ifndef EFFECT_RUNNER_T
#define EFFECT_RUNNER_T

#include <stdbool.h>
#include "effect.h"

typedef struct effect_runner effect_runner_t;

struct effect_runner {
  //OpenPixelConnect *opc;
  effect_t *effect;
  float minTime_delta;
  float current_delay;
  float speed;
};

effect_runner_t *er_new(void);
void er_free(effect_runner_t *self);

int er_set_server(effect_runner_t *self, char *hostport);

bool er_has_layout(effect_runner_t *self);
layout_t *er_get_layout(effect_runner_t *self);
int er_set_layout(effect_runner_t *self, char *filename);

effect_t *er_get_effect(effect_runner_t *self);

vector_t *er_get_pixels(effect_runner_t *self);
pixel_t er_get_pixel(effect_runner_t *self, int index);

int er_doFrame(effect_runner_t *self, float time_delta);
int er_run(effect_runner_t *self);
#endif
