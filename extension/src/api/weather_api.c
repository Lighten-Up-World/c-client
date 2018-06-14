//
// Created by User on 14/06/2018.
//

#include "weather_api.h"

//TEMPERATURE

int temp_get_pixel_for_xy(pixel_t *pixel, void *obj) {
  assert(obj != NULL);

  grid_t *grid = (grid_t *) obj;
  pixel->grid.x = grid->x;
  pixel->grid.y = grid->y;

  printf("%d \n", grid->x);
  printf("%d \n", grid->y);

  int sockfd = socket_connect(WEATHER_HOST, 80);

  geolocation_t geoloc = grid_geolocation(pixel->grid.x, pixel->grid.y);

  //TODO: REMOVE THIS WHEN DAN'S CODE WORKS:
  geoloc.latitude = (rand() % 90) * (rand() % 2 ? -1 : 1);
  geoloc.longitude = (rand() % 180) * (rand() % 2 ? -1 : 1);

  double val;
  if (get_value_for_geolocation(sockfd,&geoloc, WEATHER_HOST, WEATHER_PATH, "temp", &val) < 0){
    return -1;
  }
  pixel->colour.blue = val;
  pixel->colour.green = val;
  pixel->colour.red = val;
  return 0;
}

//WINDSPEED

int windspeed_get_pixel_for_xy(pixel_t *pixel, void *obj){
 /* grid_t *grid = (grid_t *) obj;
  pixel->grid = *grid;

  geolocation_t geoloc = grid_geolocation(pixel->grid.x, pixel->grid.y);
*/
  return 0;
}
