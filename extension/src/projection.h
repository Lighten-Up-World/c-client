#ifndef PROJECTION_H
#define PROJECTION_H

#include <math.h>
#include "apimanager.h"
#include "pixel.h"

#define EARTH_RADIUS 6378137
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif


double merc_x (double lon);
double merc_y (double lat);
double merc_lon (double x);
double merc_lat (double y);

grid_t geolocation2grid(double latitude, double longitude);
geolocation_t grid2geolocation(int x, int y);

#endif
