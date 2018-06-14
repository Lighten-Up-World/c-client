#ifndef PROJECTION_H
#define PROJECTION_H

#include <math.h>
#include "apimanager.h"
#include "pixel.h"

#define R_MAJOR 6378137.0
#define R_MINOR 6356752.3142
#define RATIO (R_MINOR/R_MAJOR)
#define ECCENT (sqrt(1.0 - (RATIO * RATIO)))
#define COM (0.5 * ECCENT)
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif



#define MAX_X 10018754.171395
#define MAX_Y 34619289.371856


double merc_x (double lon);
double merc_y (double lat);
double merc_lon (double x);
double merc_lat (double y);

grid_t geolocation_grid(double latitude, double longitude);
geolocation_t grid_geolocation(double x, double y);

#endif
