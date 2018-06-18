#ifndef SUNRISE_H
#define SUNRISE_H

#include "../apimanager.h"
#include "../effect_runner.h"

// Attribution to https://sunrise-sunset.org/
// Sample request: https://api.sunrise-sunset.org/json?lat=36.7201600&lng=-4.4203400

#define SUNRISE_HOST "api.sunrise-sunset.org"
#define SUNRISE_PATH "/json?lat=%f&lng=%f" // can also add &date and &formatted


#endif
