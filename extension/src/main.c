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
#define GEOLOC_FILE "layout/GeoLocToCoords.txt"

#define HOST_AND_PORT "127.0.0.1:7890"

typedef struct {
  grid_t grid;
  int pos;
} grid_pos_t;

typedef struct {
  grid_t grid;
  geolocation_t geo;
} grid_geo_t;


volatile int interrupted = 0;

void handle_user_exit(int _) {
  interrupted = 1;
}

int init_grid_pos(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(PIXEL_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);

      grid_pos_t *pair = malloc(sizeof(grid_pos_t));
      *pair = (grid_pos_t){{.x = atoi(rowFields[0]), .y =atoi(rowFields[1])},
              .pos = atoi(rowFields[2])};
      list_add(list, pair);

      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int init_grid_geo(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(GEOLOC_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);

      grid_geo_t *pair = malloc(sizeof(grid_geo_t));
      *pair = (grid_geo_t){{.x = atoi(rowFields[2]), .y = atoi(rowFields[3])},
              {.latitude = atof(rowFields[0]), .longitude = atof(rowFields[1])}};
      list_add(list, pair);

      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int main(int argc, const char * argv[]) {
  assert(argc > 1);
  pixel pixels[NUM_PIXELS];

  uint8_t channel = 0;
  opc_sink s;
  // Open connection
  s = opc_new_sink(HOST_AND_PORT);

  opc_put_pixels(s, channel, NUM_PIXELS, pixels);

  for(int p = 0; p < NUM_PIXELS; p++) {
    pixels[p] = (pixel){PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX,PIXEL_COLOUR_MAX};
  }

  FILE *file = fopen(PIXEL_FILE, "r");
  if (file == NULL) {
    perror("File could not be opened");
    return -1;
  }

  FILE *loc_file = fopen(GEOLOC_FILE, "r");
  if (loc_file == NULL) {
    perror("File could not be opened");
    return -1;
  }

  list_t *grid_pos = list_new(&free);
  init_grid_pos(grid_pos);

  list_t *grid_geo = list_new(&free);
  init_grid_geo(grid_geo);

  for (int i = 0; i < NUM_PIXELS; i++) {
    geolocation_t geo = ((grid_geo_t *)list_get(grid_geo, i))->geo;
    pixel_t pix;
    pix.grid = ((grid_pos_t *)list_get(grid_pos, i))->grid;

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
  opc_close(s);
  return 0;
}
