#include "projection.h"

static double deg2rad (double ang) {
  return ang * (M_PI / 180.0);
}

static double rad2deg (double ang) {
  return ang * (180.0 / M_PI);
}

// METER BASED FUNCTIONS
/*
 * I don't think these are right
 */
double lon2x_m (double lon) {
  return deg2rad(lon) * EARTH_RADIUS;
}

double lat2y_m (double lat) {
  return log(tan( deg2rad(lat) / 2 + M_PI/4 )) * EARTH_RADIUS;
}

double x2lon_m(double x) {
  return rad2deg(x/EARTH_RADIUS);
}

double y2lat_m(double y) {
  return rad2deg(2 * atan(exp( y/EARTH_RADIUS)) - M_PI/2);
}


// GRID REFERENCE BASED FUNCTIONS

// 'Constant' functions

/*
 * Think these should be changed somewhat too since our map loses 30 degrees
 * of latitude at the bottom for antarctica.
 */
double max_x_m(void){
  return lon2x_m(180);
}

double max_y_m(void){
  return lat2y_m(90);
}

double offset_x_m(void){
  return lon2x_m(-180);
}

double offset_y_m(void){
  return lat2y_m(-90);
}

double gridx2m(int x){
  double max_x = max_x_m();
  return (((double)x * 2 * max_x) / GRID_WIDTH) - max_x;
}

double gridy2m(int y){
  double max_y = max_y_m();
  return (((double)y * 2 * max_y) / GRID_HEIGHT) - max_y;
}

int lon2gridx(double lon){
  double max_x = max_x_m();
  return (lon2x_m(lon) + max_x) / (2 * max_x / (double)GRID_WIDTH);
}

int lat2gridy(double lat){
  double max_y = max_y_m();
  return (lat2y_m(lat) + max_y) / (2 * max_y / (double)GRID_HEIGHT);
}

grid_t geolocation2grid(double latitude, double longitude){
  return (grid_t){lon2gridx(longitude), lat2gridy(latitude)};
}

geolocation_t grid2geolocation(int x, int y){
  return (geolocation_t){y2lat_m(gridy2m(y)), x2lon_m(gridx2m(x))};
}
