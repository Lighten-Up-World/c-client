//
//  main.c
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//

#include <stdio.h>
#include "apimanager.h"
#include <unistd.h>

int main(int argc, const char * argv[]) {

  //Placeholder main.


  char *attr = "temp";
  geolocation_t locs[100];
  for (int j = 0; j < 100; j++) {
    sleep(1);
    locs[j] = (geolocation_t) {.latitude = rand() % 90, .longitude = rand() % 180, .value = 0};

    if (get_value_for_geolocation(locs+j,WEATHER_HOST, WEATHER_PATH, attr) < 0) {
      continue;
    }
    printf("Latitude: %f, Longitude: %f, Value: %f \n", locs[j].latitude, locs[j].longitude, locs[j].value);
  }

  return 0;
}
