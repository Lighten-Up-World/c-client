//
//  main.c
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include "apimanager.h"
#include "projection.h"

int main(int argc, const char * argv[]) {

  //Placeholder main.


  char *attr = "temp";
  geolocation_t locs[100];
  printf("MAX X: %f, MAX Y: %f\n", merc_x(180), merc_y(90));
  printf("MIN X: %f, MIN Y: %f\n", merc_x(-180), merc_y(-90));
  for (int j = 0; j < 100; j++) {
    sleep(1);
    locs[j] = (geolocation_t) {.latitude = -90, .longitude = -180, .value = 0};

    if (get_value_for_geolocation(locs+j,WEATHER_HOST, WEATHER_PATH, attr) < 0) {
      continue;
    }
    printf("Latitude: %f, Longitude: %f, Value: %f \n", locs[j].latitude, locs[j].longitude, locs[j].value);
    grid_t gref = geolocation_grid(locs[j].longitude, locs[j].latitude);
    printf("X: %d, Y: %d\n", gref.x,  gref.y);
  }

  return 0;
}
