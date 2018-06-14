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

  pixel_t pixels[100];
  for (int i = 0; i < 10 ; ++i) {
    sleep(1);
    grid_t grid = {.x = i, .y = i};
    if(temp_get_pixel_for_xy(&pixels[i],&grid) < 0){
      printf("Failed \n");
    }else{
      printf("x: %d, y: %d, r: %d, g: %d, b: %d \n", pixels[i].grid.x, pixels[i].grid.y, pixels[i].colour.red, pixels[i].colour.green, pixels[i].colour.blue);
    }
  }

  api_manager_delete(api_manager);

  return 0;
}
