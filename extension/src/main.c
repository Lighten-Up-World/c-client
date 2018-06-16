//
//  main.c
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "apimanager.h"
#include "api/weather_api.h"
#include "projection.h"
#include "opc/opc.h"
#include "opc/opc_client.c"
#include "utils/csv.h"
#include "utils/list.h"

// Grid size
#define MAX_X_GRID 52
#define MAX_Y_GRID 24
#define NUM_PIXELS 471

#define PIXEL_FILE "layout/CoordsToListPos.txt"
#define GEOLOC_FILE "layout/GeoLocToListPos.txt"

#define HOST_AND_PORT "127.0.0.1:7890"

volatile int interrupted = 0;

void handle_user_exit(int _) {
  interrupted = 1;
}

int init_grid(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(PIXEL_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);
      int pos = atoi(rowFields[2]);
      pixel_info_t *pi;
      if((pi = list_get(list, pos)) == NULL){
        pi = malloc(sizeof(pixel_info_t));
        *pi = (pixel_info_t){{.x = atoi(rowFields[0]), .y =atoi(rowFields[1])},
                {-1, -1}};
        list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
      }
      else {
        pi->grid = (grid_t){.x = atoi(rowFields[0]), .y =atoi(rowFields[1])};
      }
      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int init_geo(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(GEOLOC_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);

      int pos = atoi(rowFields[2]);
      pixel_info_t *pi;
      if((pi = list_get(list, pos)) == NULL){
        pi = malloc(sizeof(pixel_info_t));
        *pi = (pixel_info_t){{-1, -1},
                {.latitude = atof(rowFields[0]), .longitude =atof(rowFields[1])}};
        list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
      }
      else {
        pi->geo = (geolocation_t){.latitude = atof(rowFields[0]), .longitude =atof(rowFields[1])};
      }

      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int main(int argc, const char * argv[]) {
  assert(argc > 1);
  pixel pixels[NUM_PIXELS];

  signal(SIGINT, handle_user_exit);

  uint8_t channel = 0;
  opc_sink s;
  // Open connection
  s = opc_new_sink(HOST_AND_PORT);

  opc_put_pixels(s, channel, NUM_PIXELS, pixels);

  //Clear Pixels
  for(int p = 0; p < NUM_PIXELS; p++) {
    pixels[p] = (pixel){PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX,PIXEL_COLOUR_MAX};
  }

  // Setup pixel_info
  list_t *pixel_info = list_new(&free);
  init_grid(pixel_info);
  init_geo(pixel_info);

  // API Call
  api_manager_t *api_manager= api_manager_new();
  api_t *api = get_temp_api();
  // Initialise api
  api_manager_init(api_manager, api, pixel_info);

  // Setup time delay
  struct timespec delay;
  delay.tv_sec = 1;
  delay.tv_nsec = 0;

  while(!interrupted){
    for (int i = 0; i < NUM_PIXELS; i++) {
      if(interrupted){
        break;
      }
      nanosleep(&delay, NULL);

      geolocation_t geo = ((pixel_info_t *)list_get(pixel_info, i))->geo;
      pixel_t pix;
      pix.grid = ((pixel_info_t *)list_get(pixel_info, i))->grid;

      if (strncmp("temp", argv[1], strlen(argv[1])) == 0){
        if(temp_get_pixel_for_xy(&pix, geo) < 0){
          printf("Failed \n");
        }
      }else if (strncmp("windspeed", argv[1], strlen(argv[1])) == 0){
        if(windspeed_get_pixel_for_xy(&pix, geo) < 0){
          printf("Failed \n");
        }
      }
      pixel p = {.r = pix.colour.red, .b = pix.colour.blue, .g = pix.colour.green};
      pixels[i] = p;
      opc_put_pixels(s, channel, NUM_PIXELS, pixels);
    }
  }

  // Close it all up
  api_manager_delete(api_manager);
  list_delete(pixel_info);
  opc_close(s);
  return 0;
}
