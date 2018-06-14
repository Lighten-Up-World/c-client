#include "projection.h"

static double deg_rad (double ang) {
        return ang * (M_PI / 180.0);
}

double merc_x (double lon) {
        return R_MAJOR * deg_rad (lon);
}

double merc_y (double lat) {
        lat = fmin (89.5, fmax (lat, -89.5));
        double phi = deg_rad(lat);
        double sinphi = sin(phi);
        double con = ECCENT * sinphi;
        con = pow((1.0 - con) / (1.0 + con), COM);
        double ts = tan(0.5 * (M_PI * 0.5 - phi)) / con;
        return 0 - R_MAJOR * log(ts);
}

static double rad_deg (double ang) {
        return ang * (180.0 / M_PI);
}

double merc_lon (double x) {
        return rad_deg(x) / R_MAJOR;
}

double merc_lat (double y) {
        double ts = exp ( -y / R_MAJOR);
        double phi = (M_PI / 2) - 2 * atan(ts);
        double dphi = 1.0;
        int i;
        for (i = 0; fabs(dphi) > 0.000000001 && i < 15; i++) {
                double con = ECCENT * sin (phi);
                dphi = (M_PI / 2) - 2 * atan (ts * pow((1.0 - con) / (1.0 + con), COM)) - phi;
                phi += dphi;
        }
        return rad_deg (phi);
}


int grid_x(double lon){
  return (merc_x(lon) + MAX_X) / (2 * (double)MAX_X / (double)GRID_WIDTH);
}

int grid_y(double lat){
  return (merc_y(lat) + MAX_Y) / (2 * (double)MAX_Y / (double)GRID_HEIGHT);
}

grid_t geolocation_grid(double latitude, double longitude){
  return (grid_t){grid_x(longitude), grid_y(latitude)};
}

geolocation_t grid_geolocation(double x, double y){
  return (geolocation_t){merc_lat(y * MAX_Y), merc_lon(x * MAX_X)};
}
