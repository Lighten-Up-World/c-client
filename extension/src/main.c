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
#include "opc.h"
#include "opc_client.c"
#include "simulation/grid_to_opc.c"

// Grid size
#define MAX_X_GRID 52
#define MAX_Y_GRID 24
#define NUM_PIXELS 471

#define CONFIG_FILE "simulation/layout/CoordsToListPos.txt"
#define HOST_AND_PORT "127.0.0.1:7890"


int main(int argc, const char * argv[]) {
  u8 channel = 0;
  pixel pixels[NUM_PIXELS];
  int pos;
  for (int x = 0; x < MAX_X_GRID ; ++x) {
    for (int y = 0; y < MAX_Y_GRID; ++y) {
      if (x + y > 10){
        break;
      }
      sleep(1);
      pixel_t pix;
      pix.grid = (grid_t){.x = x, .y = y};
      if(temp_get_pixel_for_xy(&pix) < 0){
        printf("Failed \n");
      }else{
        printf("x: %d, y: %d, r: %d, g: %d, b: %d \n", pix.grid.x, pix.grid.y, pix.colour.red, pix.colour.green, pix.colour.blue);
      }
      pixel p = {.r = pix.colour.red, .b = pix.colour.blue, .g = pix.colour.green};
      pos = get_pixel_location(x, y, CONFIG_FILE);
      if (pos != -1) {
        pixels[pos] = p;
      }
    }
  }
  // Open connection
  opc_sink sink = opc_new_sink(HOST_AND_PORT);
  // Update the display
  opc_put_pixels(sink, channel, NUM_PIXELS, pixels);

  return 0;
}
