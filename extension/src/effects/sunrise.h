#ifndef SUNRISE_H
#define SUNRISE_H

#include "../apimanager.h"
#include "../effect_runner.h"

// Attribution to https://sunrise-sunset.org/
// Sample request: https://api.sunrise-sunset.org/json?lat=36.7201600&lng=-4.4203400

#define SUNRISE_HOST "api.sunrise-sunset.org"
#define SUNRISE_PATH "/json?lat=%f&lng=%f%d" // can also add &date and &formatted

#define SUN_FILE "sun_data.txt"

typedef struct {
    FILE *sun_file;
    struct tm *current_time;
} sun_state_t;

int sun_get_pixel(effect_runner_t *self, int pos);
effect_t *get_sun_effect(void * obj);

#endif
