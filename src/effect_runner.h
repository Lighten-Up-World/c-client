#ifndef EFFECT_RUNNER_H
#define EFFECT_RUNNER_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "opc/opc_client.h"
#include "utils/csv.h"
#include "extension.h"
#include "webctrl/examples/gnuserver.h"

#define PIXEL_FILE "layout/coordinates.txt"
#define GEOLOC_FILE "layout/geolocation.txt"
#define STRIP_FILE "layout/strip_config.txt"

effect_runner_t *effect_runner_new(void);
void effect_runner_delete(effect_runner_t *self);
effect_runner_t *effect_runner_init(effect_runner_t *self, effect_t *effect, list_t *pixel_info, opc_sink sink);

// EFFECTS
#include "effects/weather.h"
#include "effects/sunrise.h"
#include "effects/scroller.h"
#include "effects/sunrise.h"
#include "effects/image.h"
#include "effects/test.h"
#include "effects/test0.h"

#endif
