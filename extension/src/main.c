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
#include "opc/opc.h"
#include "opc/opc_client.c"

// Grid size
#define MAX_X_GRID 52
#define MAX_Y_GRID 24
#define NUM_PIXELS 471

#define CONFIG_FILE "simulation/layout/CoordsToListPos.txt"
#define HOST_AND_PORT "127.0.0.1:7890"

volatile int interrupted = 0;

void handle_user_exit(int _) {
  interrupted = 1;
}


int main(int argc, const char * argv[]) {
  assert(argc > 1);
  pixel pixels[NUM_PIXELS];

  uint8_t channel = 0;
  opc_sink s;
  // Open connection
  s = opc_new_sink("127.0.0.1:7890");

  opc_put_pixels(s, channel, NUM_PIXELS, pixels);

  for(int p = 0; p < NUM_PIXELS; p++) {
    pixels[p] = (pixel){PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX,PIXEL_COLOUR_MAX};
  }

  FILE *file = fopen(CONFIG_FILE, "r");
  if (file == NULL) {
    perror("File could not be opened");
    return -1;
  }

  int location;
  size_t line_size = 100;
  char buffer[100];
  int count = 0;

  signal(SIGINT, handle_user_exit);

  while (!interrupted){
    fseek(file, 0, SEEK_SET);
    while (fgets(buffer, (int) line_size, file) != NULL && !interrupted){

      sleep(1);
      int x = atoi(strtok(buffer, " "));
      int y = atoi(strtok(NULL, " "));

      pixel_t pix;
      pix.grid = (grid_t){.x = x, .y = y};

      if (strncmp("temp", argv[1], strlen(argv[1])) == 0){
        if(temp_get_pixel_for_xy(&pix) < 0){
          printf("Failed \n");
        }else{
          printf("x: %d, y: %d, r: %d, g: %d, b: %d \n", pix.grid.x, pix.grid.y, pix.colour.red, pix.colour.green, pix.colour.blue);
        }
      }else if (strncmp("windspeed", argv[1], strlen(argv[1])) == 0){
        if(windspeed_get_pixel_for_xy(&pix) < 0){
          printf("Failed \n");
        }else{
          printf("x: %d, y: %d, r: %d, g: %d, b: %d \n", pix.grid.x, pix.grid.y, pix.colour.red, pix.colour.green, pix.colour.blue);
        }
      }
      location = atoi(strtok(NULL, " "));
      pixel p = {.r = pix.colour.red, .b = pix.colour.blue, .g = pix.colour.green};
      pixels[location] = p;
      opc_put_pixels(s, channel, NUM_PIXELS, pixels);
      count ++;
    }
  }
  opc_close(s);
  return 0;
}
