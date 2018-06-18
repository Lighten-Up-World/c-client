#ifndef EFFECT_RUNNER_H
#define EFFECT_RUNNER_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "opc/opc_client.c"
#include "utils/csv.h"
#include "extension.h"

#define PIXEL_FILE "layout/CoordsToListPos.txt"
#define GEOLOC_FILE "layout/GeoLocToListPos.txt"

effect_runner_t *effect_runner_new(void);
void effect_runner_delete(effect_runner_t *self);
effect_runner_t *effect_runner_init(effect_runner_t *self, effect_t *effect, list_t *pixel_info);

// EFFECTS
#include "effects/weather.h"

#endif
