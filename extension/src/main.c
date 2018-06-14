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

  char hostName[strlen(WEATHER_HOST) + 1];
  strncpy(hostName, WEATHER_HOST, strlen(WEATHER_HOST));
  hostName[strlen(WEATHER_HOST)] = 0;

  char pathName[strlen(WEATHER_PATH) + 1];
  strncpy(pathName, WEATHER_PATH, strlen(WEATHER_PATH));
  pathName[strlen(WEATHER_PATH)] = 0;


  char *attr = "temp";
  geolocation_t locs[100];
  for (int j = 0; j < 100; j++) {
    sleep(1);
    locs[j] = (geolocation_t) {.latitude = rand() % 90, .longitude = rand() % 180, .value = 0};

    if (get_value_for_geolocation(locs+j,hostName, pathName, attr) < 0) {
      continue;
    }
    printf("Latitude: %d, Longitude: %d, Value: %f \n", locs[j].latitude, locs[j].longitude, locs[j].value);
    strncpy(pathName + LATITUDE_START, "000", 3);
    strncpy(pathName + LONGITUDE_START, "000", 3);
  }

  return 0;
}
