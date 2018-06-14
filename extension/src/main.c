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
#include "api/weather_api.h"
#include "projection.h"

int main(int argc, const char * argv[]) {

  api_manager_t *api_manager = api_manager_new();

  temp_construct(api_manager);

  pixel_t pixels[100];
  for (int i = 0; i < 100 ; ++i) {
    geolocation_t geoloc = {.longitude = rand() % 10, .latitude = rand() % 12};
    if(temp_get_pixel_for_xy(api_manager, &pixels[i],&geoloc) < 0){
      printf("Failed");
    }
    printf("x: %d, y: %d, r: %d, g: %d, b: %d \n", pixels[i].grid.x, pixels[i].grid.y, pixels[i].colour.red, pixels[i].colour.green, pixels[i].colour.blue);
  }

  temp_destruct(api_manager);
  api_manager_delete(api_manager);

  return 0;
}
